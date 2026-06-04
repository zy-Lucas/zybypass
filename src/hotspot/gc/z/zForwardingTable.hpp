#pragma once

#include "runtime/jvmObject.hpp"
#include "zAddress.hpp"
#include "zForwarding.hpp"
#include "zGranuleMapForForwarding.hpp"

namespace hotspot::gc::z
{
class ZForwardingTable : public runtime::JvmObject
{
  public:
    ZForwardingTable(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    ZForwarding forwarding(uint64_t offset) const noexcept { return map().forwarding(ZAddress::offset(offset)); }

  private:
    ZGranuleMapForForwarding map() const noexcept { return address() + map_offset_; }

    DECLARE_STATIC_INIT

    static inline uint64_t map_offset_;
};
} // namespace hotspot::gc::z