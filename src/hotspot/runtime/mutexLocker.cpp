#include "mutexLocker.hpp"
#include "runtime/mutex.hpp"
#include "runtime/thread.hpp"

namespace hotspot::runtime
{
MutexLocker::MutexLocker(Mutex mutex, bool is_safepoint_check) noexcept : mutex_(mutex)
{
    if (!mutex_)
        return;
    if (is_safepoint_check)
    {
        // mutex_.lock();
        return;
    }
    mutex_.lock_without_safepoint_check();
}

MutexLocker::MutexLocker(Thread thread, Mutex mutex, bool is_safepoint_check) noexcept : mutex_(mutex)
{
    if (!mutex_)
        return;
    if (is_safepoint_check)
    {
        // mutex_.lock(thread);
        return;
    }
    mutex_.lock_without_safepoint_check(thread);
}

MutexLocker::~MutexLocker() noexcept
{
    if (!mutex_)
        return;
    mutex_.unlock();
}
} // namespace hotspot::runtime