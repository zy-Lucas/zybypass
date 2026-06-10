#include "zForwarding.hpp"

namespace hotspot::gc::z
{
ZForwardingEntry ZForwarding::at(uint64_t cursor) const noexcept
{
    return entries().entry(*this).address() + ZForwardingEntry::size() * cursor;
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
    : mask_(forwarding->entries().length() - 1), cursor_(ZForwarding::uint32_to_uint32(from_index) & mask_),
      forwarding_(forwarding), next_entry_(forwarding->at(cursor_))
{
}

ZForwardingEntry ZForwarding::ZForwardEntryIterator::next() noexcept
{
    ZForwardingEntry entry{next_entry_};
    cursor_ = (cursor_ + 1) & mask_;
    next_entry_ = forwarding_->at(cursor_);
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
    utils::FieldResolver r{"ZForwarding"};
    
    r.field_offset("_virtual", virtual_offset_);
    r.field_offset("_entries", entries_offset_);
    r.field_offset("_object_alignment_shift", object_alignment_shift_offset_);
    r.field_offset("_ref_count", ref_count_offset_);

    type_ = r.type();
}
} // namespace hotspot::gc::z