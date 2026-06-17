#pragma once

#include "nmethod.hpp"

namespace hotspot::code
{
class nmethodBucket : public runtime::JvmObject
{
  public:
    nmethodBucket(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    nmethod nmethod() const noexcept { return read_field<uint64_t>(nmethod_offset_); }
    void set_nmethod(class nmethod nm) noexcept { write_field(nmethod_offset_, nm.address()); }

    int32_t count() const noexcept { return read_field<int32_t>(count_offset_); }
    void set_count(int32_t count) noexcept { write_field(count_offset_, count); }

    nmethodBucket next() const noexcept { return read_field<uint64_t>(next_offset_); }
    nmethodBucket purge_list_next() const noexcept { return read_field<uint64_t>(purge_list_next_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t nmethod_offset_;
    static inline uint64_t count_offset_;
    static inline uint64_t next_offset_;
    static inline uint64_t purge_list_next_offset_;
};
} // namespace hotspot::code