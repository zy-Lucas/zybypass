#pragma once

#include "../../runtime/jvmObject.hpp"

namespace hotspot::gc::z
{
class ZVirtualMemory : public runtime::JvmObject
{
  public:
    ZVirtualMemory(uint64_t addr) : runtime::JvmObject(addr) {}

    uint64_t start() const noexcept { return read_field<uint64_t>(start_offset); }
    uint64_t end() const noexcept { return read_field<uint64_t>(end_offset); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t start_offset;
    static inline uint64_t end_offset;
};
} // namespace hotspot::gc::z