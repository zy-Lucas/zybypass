#include "zRelocate.hpp"
#include "zAddress.hpp"

namespace hotspot::gc::z
{
uint64_t ZRelocate::forwarding_find(ZForwarding forwarding, uint64_t from) noexcept
{
    if (ZForwardingEntry entry = forwarding.find(forwarding_index(forwarding, from)); entry.populated())
        return ZAddress::good(entry.to_offset());
    return 0;
}

uint64_t ZRelocate::relocate_object(ZForwarding forwarding, uint64_t o) noexcept
{
    if (uint64_t to_addr = forwarding_find(forwarding, o); to_addr)
        return to_addr;
    return o;
}

uint64_t ZRelocate::forwarding_index(ZForwarding forwarding, uint64_t from) noexcept
{
    return (ZAddress::offset(from) - forwarding.start()) >> forwarding.object_alignment_shift();
}

void ZRelocate::initialize()
{
    // types::Type *type = runtime::Jvm::lookup_type("ZRelocate");
}
} // namespace hotspot::gc::z