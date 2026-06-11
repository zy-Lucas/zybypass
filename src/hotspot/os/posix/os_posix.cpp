#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include "os_posix.hpp"
#include "os/os.hpp"
#include "runtime/jvm.hpp"
#include "utilities/globalDefinitions.hpp"
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace hotspot::os
{
static constexpr int64_t MAX_SECS = 100000000;
static constexpr int64_t MAX_WAIT_MILLIS = MAX_SECS * MILLIUNITS;

void *chop_extra_memory(uint64_t size, uint64_t alignment, void *extra_base, uint64_t extra_size)
{
    char *base = (char *)extra_base;
    char *aligned = (char *)runtime::Jvm::align_up((uint64_t)base, alignment);

    uint64_t prefix = uint64_t(aligned - base);
    uint64_t suffix = extra_size - prefix - size;

    if (prefix)
        os::release_memory(base, prefix);
    if (suffix)
        os::release_memory(aligned + size, suffix);
    return aligned;
}

void *os::reserve_memory_aligned(uint64_t size, uint64_t alignment, bool exec)
{
    uint64_t extra_size = size + alignment;
    void *extra_base = reserve_memory(extra_size, exec);
    if (!extra_base)
        return nullptr;
    return chop_extra_memory(size, alignment, extra_base, extra_size);
}

void *reserve_mmapped_memory(uint64_t bytes, void *requested_addr)
{
    int32_t flags = MAP_PRIVATE | MAP_ANONYMOUS;
#ifndef _AIX
    flags |= MAP_NORESERVE;
#endif
    if (requested_addr)
        flags |= MAP_FIXED;
    if (void *addr = mmap(requested_addr, bytes, PROT_NONE, flags, -1, 0); addr != MAP_FAILED)
        return addr;
    return nullptr;
}

int32_t util_posix_fallocate(int32_t fd, int64_t offset, int64_t len)
{
#ifdef __APPLE__
    fstore_t store{F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, len};
    int32_t ret = fcntl(fd, F_PREALLOCATE, &store);
    if (ret == -1)
    {
        store.fst_flags = F_ALLOCATEALL;
        ret = fcntl(fd, F_PREALLOCATE, &store);
    }
    if (ret != -1)
        return ftruncate(fd, len);
    return -1;
#else
    return posix_fallocate(fd, offset, len);
#endif
}

void *os::map_memory_to_file(void *base, uint64_t size, int32_t fd)
{
    if (util_posix_fallocate(fd, 0, (off_t)size))
        return nullptr;
    int32_t prot = PROT_READ | PROT_WRITE;
    int32_t flags = MAP_SHARED;
    if (base)
        flags |= MAP_FIXED;
    void *addr = mmap(base, size, prot, flags, fd, 0);
    if (addr == MAP_FAILED)
        return nullptr;
    if (base && addr != base)
    {
        os::release_memory(addr, size);
        return nullptr;
    }
    return addr;
}

void *os::map_memory_to_file_aligned(uint64_t size, uint64_t alignment, int32_t fd)
{
    uint64_t extra_size = size + alignment;
    void *extra_base = reserve_mmapped_memory(extra_size, NULL);
    if (!extra_base)
        return nullptr;
    void *aligned_base = chop_extra_memory(size, alignment, extra_base, extra_size);
    if (!map_memory_to_file(aligned_base, size, fd))
        abort();
    return aligned_base;
}

bool cond_uses_monotonic()
{
    using setclock_t = int (*)(pthread_condattr_t *, clockid_t);
    setclock_t fn = (setclock_t)dlsym(RTLD_DEFAULT, "pthread_condattr_setclock");
    if (!fn)
        return false;
    pthread_condattr_t tmp;
    if (pthread_condattr_init(&tmp))
        return false;
    int32_t status = fn(&tmp, CLOCK_MONOTONIC);
    pthread_condattr_destroy(&tmp);
    if (!status)
        return true;
    if (status == EINVAL)
        return false;
    fprintf(stderr, "fatal: pthread_condattr_setclock(CLOCK_MONOTONIC) failed: %s (errno=%d)\n", strerror(status),
            status);
    abort();
}

static void calc_rel_time(timespec *abstime, int64_t timeout_ns, int64_t now_sec, int64_t now_nsec)
{
    int64_t seconds = timeout_ns / NANOUNITS;
    int64_t nanos = timeout_ns % NANOUNITS;

    if (seconds >= MAX_SECS)
    {
        abstime->tv_sec = now_sec + MAX_SECS;
        abstime->tv_nsec = 0;
        return;
    }

    abstime->tv_sec = now_sec + seconds;
    nanos += now_nsec;
    if (nanos >= NANOUNITS)
    {
        abstime->tv_sec += 1;
        nanos -= NANOUNITS;
    }
    abstime->tv_nsec = nanos;
}

static void unpack_abs_time(timespec *abstime, int64_t deadline_ms, int64_t now_sec)
{
    int64_t seconds = deadline_ms / MILLIUNITS;
    int64_t millis = deadline_ms % MILLIUNITS;

    if (seconds >= now_sec + MAX_SECS)
    {
        abstime->tv_sec = now_sec + MAX_SECS;
        abstime->tv_nsec = 0;
        return;
    }

    abstime->tv_sec = seconds;
    abstime->tv_nsec = millis_to_nanos(millis);
}

static void to_abstime(timespec *abstime, int64_t timeout, bool is_absolute, bool is_realtime)
{
    timeout = std::max<int64_t>(timeout, 0);

    static const bool use_monotonic = cond_uses_monotonic();
    const clockid_t clock{(is_absolute || !use_monotonic || is_realtime) ? CLOCK_REALTIME : CLOCK_MONOTONIC};

    timespec now;
    clock_gettime(clock, &now);

    if (!is_absolute)
    {
        calc_rel_time(abstime, timeout, now.tv_sec, now.tv_nsec);
        return;
    }

    unpack_abs_time(abstime, timeout, now.tv_sec);
}

int32_t PlatformMonitor::wait(int64_t millis)
{
    if (millis < 0)
        return OS_ERR;
    if (millis > 0)
    {
        timespec abst;
        millis = std::min(millis, MAX_WAIT_MILLIS);
        to_abstime(&abst, millis_to_nanos(millis), false, false);

        int ret = OS_TIMEOUT;
        if (!pthread_cond_timedwait(cond(), mutex(), &abst))
            ret = OS_OK;
        return ret;
    }

    pthread_cond_wait(cond(), mutex());
    return OS_OK;
}
} // namespace hotspot::os
#endif