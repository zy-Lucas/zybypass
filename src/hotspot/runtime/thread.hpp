#pragma once

#include "jvmObject.hpp"

namespace hotspot::runtime
{
class Thread : public JvmObject
{
  public:
    Thread(uint64_t addr) : JvmObject(addr) {}

    uint32_t suspend_flags() const noexcept { return read_field<uint32_t>(suspend_flags_offset_); }
    bool has_async_exception() const noexcept { return (suspend_flags() & has_async_exception_) != 0; }

    int64_t allocated_bytes() const noexcept { return read_field<int64_t>(allocated_bytes_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t tlab_field_offset_;
    static inline uint64_t active_handles_offset_;
    static inline uint64_t allocated_bytes_offset_;

    static inline uint32_t has_async_exception_;

    static inline uint64_t suspend_flags_offset_;
    static inline uint64_t current_pending_monitor_offset_;
    static inline uint64_t current_waiting_monitor_offset_;
};
} // namespace hotspot::runtime