#pragma once

#include "runtime/jvmObject.hpp"
#include "oop.hpp"

namespace hotspot::oops
{
class VMOopHandle : runtime::JvmObject
{
  public:
    VMOopHandle(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t obj() const noexcept { return read_field<uint64_t>(obj_offset); }

    Instance resolve() const;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t obj_offset;
};
} // namespace hotspot::oops