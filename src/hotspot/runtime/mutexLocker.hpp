#pragma once

#include "runtime/mutex.hpp"

namespace hotspot::runtime
{
class MutexLocker
{
  public:
    MutexLocker(Mutex mutex, bool is_safepoint_check = true) noexcept;
    MutexLocker(Thread thread, Mutex mutex, bool is_safepoint_check_flag = true) noexcept;

    ~MutexLocker() noexcept;

  protected:
    Mutex mutex_;
};
} // namespace hotspot::runtime