#pragma once

#include "../../runtime/jvmObject.hpp"
#include "zAddress.hpp"
#include "zForwarding.hpp"
#include "zGranuleMapForForwarding.hpp"

namespace hotspot::gc::z
{
class ZForwardingTable : public runtime::JvmObject
{
  public:
    ZForwardingTable(uint64_t addr) : runtime::JvmObject(addr) {}

    ZForwarding get(uint64_t o) const noexcept { return map().get(ZAddress::offset(o)); }

  private:
    ZGranuleMapForForwarding map() const noexcept { return address() + map_offset; }

    DECLARE_STATIC_INIT

    static inline uint64_t map_offset;
};
} // namespace hotspot::gc::z