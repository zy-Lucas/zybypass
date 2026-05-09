#include "zForwarding.hpp"

namespace hotspot::gc::z
{
ZForwardingEntry ZForwarding::at(uint64_t cursor) const noexcept
{
    return entries().get(*this).address() + ZForwardingEntry::get_size() * cursor;
}

ZForwardingEntry ZForwarding::find(uint64_t from_index) noexcept
{
    ZForwardEntryIterator itr{from_index, this};
    while (itr.has_next())
        if (ZForwardingEntry entry{itr.next()}; entry.from_index() == from_index)
            return entry;
    return itr.peak();
}

ZForwarding::ZForwardEntryIterator::ZForwardEntryIterator(uint64_t from_index, ZForwarding *forwarding) noexcept
    : mask(forwarding->entries().length() - 1), cursor(ZForwarding::uint32_to_uint32(from_index) & mask),
      forwarding(forwarding), next_entry(forwarding->at(cursor))
{
}

ZForwardingEntry ZForwarding::ZForwardEntryIterator::next() noexcept
{
    ZForwardingEntry entry{next_entry};
    cursor = (cursor + 1) & mask;
    next_entry = forwarding->at(cursor);
    return entry;
}

uint32_t ZForwarding::uint32_to_uint32(uint32_t key) noexcept
{
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
    return key;
}

void ZForwarding::initialize()
{
    type = runtime::Jvm::lookup_type("ZForwarding");

    virtual_offset = *type->get_field_offset("_virtual");
    entries_offset = *type->get_field_offset("_entries");
    object_alignment_shift_offset = *type->get_field_offset("_object_alignment_shift");
    ref_count_offset = *type->get_field_offset("_ref_count");
}
} // namespace hotspot::gc::z