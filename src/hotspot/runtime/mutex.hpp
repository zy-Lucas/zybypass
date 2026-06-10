#pragma once

#include "runtime/threads.hpp"

#ifndef _WIN32
#include "os/posix/os_posix.hpp"
#else
#endif

namespace hotspot::runtime
{
class Mutex : public JvmObject
{
  public:
    Mutex(uint64_t addr) noexcept : JvmObject(addr) {}

    Thread owner() const noexcept { return atomic_load_field<uint64_t>(owner_offset_, std::memory_order_relaxed); }
    void set_owner(Thread thread) noexcept;

    os::PlatformMonitor *locker() const noexcept { return (os::PlatformMonitor *)(address() + lock_offset_); }

    void lock() {}              // unfinished
    void lock(Thread thread) {} // unfinished
    void unlock() noexcept;
    bool is_locked() const noexcept { return (bool)owner(); }

    bool try_lock() { return false; } // unfinished

    void lock_without_safepoint_check() noexcept { lock_without_safepoint_check(Threads::current()); }
    void lock_without_safepoint_check(Thread self) noexcept;

    std::string_view name_view() const noexcept { return read_string_field(name_offset_); }
    std::string name() const { return std::string{name_view()}; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t owner_offset_;
    static inline uint64_t lock_offset_;
    static inline uint64_t name_offset_;
};
} // namespace hotspot::runtime