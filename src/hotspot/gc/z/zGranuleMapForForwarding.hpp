#pragma once

#include "../../runtime/jvmObject.hpp"
#include "zGlobals.hpp"

namespace hotspot::gc::z
{
class ZGranuleMapForForwarding : public runtime::JvmObject
{
  public:
    ZGranuleMapForForwarding(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t forwarding_at(uint64_t index) const noexcept;
    uint64_t forwarding(uint64_t offset) const noexcept { return forwarding_at(index_for_offset(offset)); }

    static uint64_t size() noexcept { return ZGlobals::z_address_offset_max_ >> ZGlobals::z_granule_size_shift_; }

  private:
    uint64_t map() const noexcept { return read_field<uint64_t>(map_offset_); }

    static uint64_t index_for_offset(uint64_t offset) noexcept { return offset >> ZGlobals::z_granule_size_shift_; }

    DECLARE_STATIC_INIT

    static inline uint64_t map_offset_;
};
} // namespace hotspot::gc::z