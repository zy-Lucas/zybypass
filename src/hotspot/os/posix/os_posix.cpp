#include "os_posix.hpp"
#include "os/os.hpp"
#include "utilities/globalDefinitions.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <dlfcn.h>

namespace hotspot::os
{
static constexpr int64_t MAX_SECS = 100000000;
static constexpr int64_t MAX_WAIT_MILLIS = MAX_SECS * MILLIUNITS;

bool cond_uses_monotonic()
{
    using setclock_t = int (*)(pthread_condattr_t *, clockid_t);
    setclock_t fn = (setclock_t)dlsym(RTLD_DEFAULT, "pthread_condattr_setclock");
    if (!fn)
        return false;
    pthread_condattr_t tmp;
    if (pthread_condattr_init(&tmp))
        return false;
    int status = fn(&tmp, CLOCK_MONOTONIC);
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