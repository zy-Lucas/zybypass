#include "mutex.hpp"

namespace hotspot::runtime
{
void Mutex::set_owner(Thread thread) noexcept
{
    atomic_store_field(owner_offset_, thread.address(), std::memory_order_relaxed);
}

void Mutex::unlock() noexcept
{
    set_owner(0);
    locker()->unlock();
}

void Mutex::lock_without_safepoint_check(Thread self) noexcept
{
    locker()->lock();
    set_owner(self);
}

void Mutex::initialize()
{
    owner_offset_ = 0;
    lock_offset_ = sizeof(void *);
#if defined(__APPLE__)
    name_offset_ = lock_offset_ + 2 * sizeof(void *);
#elif defined(__linux__)
    name_offset_ = lock_offset_ + 88;
#elif defined(_WIN32)
    name_offset_ = lock_offset_ + 48;
#endif
}
} // namespace hotspot::runtime