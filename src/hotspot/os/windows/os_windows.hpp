#ifdef _WIN32
#pragma once

#include <windows.h>

namespace hotspot::os
{
class PlatformMutex
{
  public:
    PlatformMutex() = delete;
    ~PlatformMutex() = delete;

    void lock() noexcept { EnterCriticalSection(&_mutex); }
    void unlock() noexcept { LeaveCriticalSection(&_mutex); }
    bool try_lock() noexcept { return TryEnterCriticalSection(&_mutex); }

  protected:
    CRITICAL_SECTION _mutex;

  private:
    PlatformMutex(const PlatformMutex &) = delete;
    PlatformMutex &operator=(const PlatformMutex &) = delete;
};

class PlatformMonitor : public PlatformMutex
{
  public:
    PlatformMonitor() = delete;
    ~PlatformMonitor() = delete;

    int32_t wait(int64_t millis) noexcept;
    void notify() noexcept { WakeConditionVariable(&_cond); }
    void notify_all() noexcept { WakeAllConditionVariable(&_cond); }

  private:
    PlatformMonitor(const PlatformMonitor &) = delete;
    PlatformMonitor &operator=(const PlatformMonitor &) = delete;

    CONDITION_VARIABLE _cond;
};
} // namespace hotspot::os
#endif