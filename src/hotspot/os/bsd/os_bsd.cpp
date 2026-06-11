#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__APPLE__)
#include "os_bsd.hpp"
#include "os/os.hpp"
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

namespace hotspot::os
{
int32_t os::vm_page_size() { return getpagesize(); }

int32_t os::vm_allocation_granularity() { return getpagesize(); }

void *anon_mmap(void *requested_addr, uint64_t bytes, bool exec) noexcept
{
    int32_t flags = MAP_PRIVATE | MAP_NORESERVE | MAP_ANONYMOUS;
#ifdef __APPLE__
    if (exec)
        flags |= MAP_JIT;
#endif

    void *addr = mmap(requested_addr, bytes, PROT_NONE, flags, -1, 0);
    return addr == MAP_FAILED ? nullptr : addr;
}

bool anon_munmap(void *addr, uint64_t size) noexcept { return !munmap(addr, size); }

void *os::pd_reserve_memory(uint64_t bytes, bool exec) { return anon_mmap(nullptr, bytes, exec); }

void *os::pd_attempt_reserve_memory_at(void *requested_addr, uint64_t bytes, bool exec)
{
    void *addr = anon_mmap(requested_addr, bytes, exec);
    if (addr == requested_addr)
        return requested_addr;
    if (addr)
        anon_munmap(addr, bytes);
    return nullptr;
}

bool os::pd_commit_memory(void *addr, uint64_t size, bool exec)
{
    int32_t prot = exec ? PROT_READ | PROT_WRITE | PROT_EXEC : PROT_READ | PROT_WRITE;
#if defined(__OpenBSD__)
    if (!mprotect(addr, size, prot))
        return true;
#elif defined(__APPLE__)
    if (exec)
    {
        if (!mprotect(addr, size, prot))
            return true;
        return false;
    }

    if (mmap(addr, size, prot, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0) != MAP_FAILED)
        return true;
#else
    if (mmap(addr, size, prot, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0) != MAP_FAILED)
        return true;
#endif
    return false;
}

bool os::pd_commit_memory(void *addr, uint64_t size, uint64_t alignment_hint, bool exec)
{
    return pd_commit_memory(addr, size, exec);
}

bool os::pd_release_memory(void *addr, uint64_t size) { return anon_munmap(addr, size); }

void *os::pd_attempt_map_memory_to_file_at(void *requested_addr, uint64_t bytes, int32_t file_desc)
{
    void *result = pd_attempt_reserve_memory_at(requested_addr, bytes, false);
    if (result && !map_memory_to_file(result, bytes, file_desc))
        abort();
    return result;
}

void *os::pd_reserve_memory_special(uint64_t bytes, uint64_t alignment, uint64_t page_size, void *req_addr, bool exec)
{
    return nullptr;
}

bool os::pd_release_memory_special(void *base, uint64_t bytes) { return false; }
} // namespace hotspot::os
#endif