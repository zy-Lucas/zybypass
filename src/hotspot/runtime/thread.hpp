#pragma once

#include "jvmObject.hpp"
#include "threadLocalAllocBuffer.hpp"

namespace hotspot::runtime
{
class Thread : public JvmObject
{
  public:
    Thread(uint64_t addr) noexcept : JvmObject(addr) {}

    ThreadLocalAllocBuffer tlab() const noexcept { return address() + tlab_field_offset_; }

    int64_t allocated_bytes() const noexcept { return read_field<int64_t>(allocated_bytes_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t tlab_field_offset_;
    static inline uint64_t active_handles_offset_;
    static inline uint64_t allocated_bytes_offset_;
};
} // namespace hotspot::runtime