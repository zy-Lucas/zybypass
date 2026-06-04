#pragma once

#include "runtime/jvmObject.hpp"
#include "zForwarding.hpp"

namespace hotspot::gc::z
{
class ZRelocate : public runtime::JvmObject
{
  public:
    ZRelocate(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    static uint64_t forwarding_find(ZForwarding forwarding, uint64_t from) noexcept;

    static uint64_t relocate_object(ZForwarding forwarding, uint64_t o) noexcept;

  private:
    static uint64_t forwarding_index(ZForwarding forwarding, uint64_t from) noexcept;

    DECLARE_STATIC_INIT
};
} // namespace hotspot::gc::z