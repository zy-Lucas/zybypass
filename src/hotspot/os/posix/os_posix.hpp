#pragma once

#include <pthread.h>

namespace hotspot::os
{
#ifdef __APPLE__
#define PLATFORM_MONITOR_IMPL_INDIRECT 1
#else
#define PLATFORM_MONITOR_IMPL_INDIRECT 0
#endif

class PlatformMutex
{
  public:
    PlatformMutex() = delete;
    ~PlatformMutex() = delete;

    void lock() noexcept { pthread_mutex_lock(mutex()); }
    void unlock() noexcept { pthread_mutex_unlock(mutex()); }
    bool try_lock() noexcept { return !pthread_mutex_trylock(mutex()); }

#if PLATFORM_MONITOR_IMPL_INDIRECT
  protected:
    pthread_mutex_t *mutex() const noexcept { return &(impl_->mutex_); }

  private:
    class Mutex
    {
      public:
        pthread_mutex_t mutex_;
        Mutex *next_;

        Mutex() = delete;
        ~Mutex() = delete;
    };

    Mutex *impl_;

#else
  protected:
    pthread_mutex_t *mutex() const noexcept { return &mutex_; }

  private:
    pthread_mutex_t mutex_;

#endif
    PlatformMutex(const PlatformMutex &) = delete;
    PlatformMutex &operator=(const PlatformMutex &) = delete;
};

class PlatformMonitor : public PlatformMutex
{
  public:
    PlatformMonitor() = delete;
    ~PlatformMonitor() = delete;

    int32_t wait(int64_t millis);
    void notify() noexcept { pthread_cond_signal(cond()); }
    void notify_all() noexcept { pthread_cond_broadcast(cond()); }

  private:
    PlatformMonitor(const PlatformMonitor &) = delete;
    PlatformMonitor &operator=(const PlatformMonitor &) = delete;

#if PLATFORM_MONITOR_IMPL_INDIRECT
    pthread_cond_t *cond() const noexcept { return &(impl_->cond_); }

    class Cond
    {
      public:
        pthread_cond_t cond_;
        Cond *next_;

        Cond() = delete;
        ~Cond() = delete;
    };

    Cond *impl_;
#else
    pthread_cond_t *cond() const noexcept { return &cond_; }

    pthread_cond_t cond_;
#endif // PLATFORM_MONITOR_IMPL_INDIRECT
};
} // namespace hotspot::os