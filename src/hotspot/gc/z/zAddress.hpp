#pragma once

#include "zGlobals.hpp"

namespace hotspot::gc::z
{
struct ZAddress
{
    static bool is_weak_bad(uint64_t value) noexcept { return value & ZGlobals::z_address_weak_bad_mask(); }
    static bool is_weak_good(uint64_t value) noexcept { return !is_weak_bad(value) && value; }
    static bool is_weak_good_or_null(uint64_t value) noexcept { return !is_weak_bad(value); }

    static uint64_t offset(uint64_t addr) noexcept { return addr & ZGlobals::z_address_offset_mask(); }
    static uint64_t good(uint64_t value) noexcept { return offset(value) | ZGlobals::z_address_good_mask(); }
    static uint64_t good_or_null(uint64_t value) noexcept { return !value ? value : good(value); }

    static bool is_in(uint64_t addr) noexcept
    {
        return std::has_single_bit(addr & ~ZGlobals::z_address_offset_mask()) &&
               addr & (ZGlobals::z_address_metadata_mask() & ~ZGlobals::z_address_metadata_finalizable());
    }
};
} // namespace hotspot::gc::z