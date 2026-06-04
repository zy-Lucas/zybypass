#pragma once

#include "runtime/jvmObject.hpp"

namespace hotspot::memory
{
class VirtualSpace : public runtime::JvmObject
{
  public:
    VirtualSpace(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t low() const noexcept { return read_field<uint64_t>(low_offset_); }
    uint64_t high() const noexcept { return read_field<uint64_t>(high_offset_); }
    uint64_t low_boundary() const noexcept { return read_field<uint64_t>(low_boundary_offset_); }
    uint64_t high_boundary() const noexcept { return read_field<uint64_t>(high_boundary_offset_); }

    uint64_t committed_size() const noexcept { return high() - low(); }
    uint64_t reserved_size() const noexcept { return high_boundary() - low_boundary(); }
    uint64_t uncommitted_size() const noexcept { return reserved_size() - committed_size(); }
    bool contains(uint64_t addr) const noexcept { return low() <= addr && addr < high(); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t low_offset_;
    static inline uint64_t high_offset_;
    static inline uint64_t low_boundary_offset_;
    static inline uint64_t high_boundary_offset_;
};
} // namespace hotspot::memory