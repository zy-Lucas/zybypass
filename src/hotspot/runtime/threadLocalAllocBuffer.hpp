#pragma once

#include "oops/oop.hpp"

namespace hotspot::runtime
{
class ThreadLocalAllocBuffer : public JvmObject
{
  public:
    ThreadLocalAllocBuffer(uint64_t addr) noexcept : JvmObject(addr) {}

    uint64_t start() const noexcept { return read_field<uint64_t>(start_offset_); }
    void set_start(uint64_t start) noexcept { write_field(start_offset_, start); }

    uint64_t top() const noexcept { return read_field<uint64_t>(top_offset_); }
    void set_top(uint64_t top) noexcept { write_field(top_offset_, top); }

    uint64_t end() const noexcept { return read_field<uint64_t>(end_offset_); }
    void set_end(uint64_t end) noexcept { write_field(end_offset_, end); }

    uint64_t allocation_end() const noexcept { return read_field<uint64_t>(allocation_end_offset_); }
    void set_allocation_end(uint64_t end) noexcept { write_field(allocation_end_offset_, end); }

    uint64_t hard_end() const noexcept { return allocation_end() + alignment_reserve(); }

    uint64_t allocate(uint64_t size) noexcept;

  private:
    static uint64_t end_reserve() noexcept;
    static uint64_t alignment_reserve() { return oops::Oop::align_object_size(end_reserve()); }

    DECLARE_STATIC_INIT

    static inline uint64_t start_offset_;
    static inline uint64_t top_offset_;
    static inline uint64_t end_offset_;
    static inline uint64_t allocation_end_offset_;
    static inline uint64_t desired_size_offset_;
};
} // namespace hotspot::runtime