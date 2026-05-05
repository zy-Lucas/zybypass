#pragma once

#include "../../runtime/jvmObject.hpp"
#include "zGlobals.hpp"

namespace hotspot::gc::z
{
class ZGranuleMapForForwarding : public runtime::JvmObject
{
  public:
    ZGranuleMapForForwarding(uint64_t addr) : runtime::JvmObject(addr) {}

    uint64_t at(uint64_t index) const noexcept { return runtime::Jvm::read<uint64_t>(map() + index * sizeof(void *)); }
    uint64_t get(uint64_t offset) const noexcept { return at(index_for_offset(offset)); }

    static uint64_t size() noexcept { return ZGlobals::z_address_offset_max >> ZGlobals::z_granule_size_shift; }

  private:
    uint64_t map() const noexcept { return read_field<uint64_t>(map_offset); }

    static uint64_t index_for_offset(uint64_t offset) noexcept { return offset >> ZGlobals::z_granule_size_shift; }

    DECLARE_STATIC_INIT

    static inline uint64_t map_offset;
};
} // namespace hotspot::gc::z