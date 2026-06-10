#include "zForwardingEntry.hpp"

namespace hotspot::gc::z
{
bool ZForwardingEntry::field_populated_decode(uint64_t value) noexcept
{
    constexpr uint64_t mask = (1ull << 1) - 1;
    return value & mask;
}

uint64_t ZForwardingEntry::field_to_offset_decode(uint64_t value) noexcept
{
    constexpr uint64_t mask = (1ull << 45) - 1;
    return value >> 1 & mask;
}

uint64_t ZForwardingEntry::field_from_index_decode(uint64_t value) noexcept
{
    constexpr uint64_t mask = (1ull << 18) - 1;
    return value >> 46 & mask;
}

void ZForwardingEntry::initialize()
{
    utils::FieldResolver r{"ZForwardingEntry"};

    r.field_offset("_entry", entry_offset_);

    type_ = r.type();
}
} // namespace hotspot::gc::z