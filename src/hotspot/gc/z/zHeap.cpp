#include "zHeap.hpp"

namespace hotspot::gc::z
{
uint64_t ZHeap::relocate_object(uint64_t addr) const noexcept
{
    if (ZForwarding forwarding{forwardingTable().forwarding(addr)}; forwarding)
        return relocate().relocate_object(forwarding, ZAddress::good(addr));
    return ZAddress::good(addr);
}

uint64_t ZHeap::remap_object(uint64_t o) const noexcept
{
    if (ZForwarding forwarding{forwardingTable().forwarding(address())}; forwarding)
        return relocate().forwarding_find(forwarding, ZAddress::good(o));
    return ZAddress::good(o);
}

void ZHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZHeap");

    page_allocator_offset_ = *type->field_offset("_page_allocator");
    page_table_offset_ = *type->field_offset("_page_table");
    forwarding_table_offset_ = *type->field_offset("_forwarding_table");
    relocate_offset_ = *type->field_offset("_relocate");
}
} // namespace hotspot::gc::z