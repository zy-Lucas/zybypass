#include "zAttachedArrayForForwarding.hpp"
#include "zForwarding.hpp"

namespace hotspot::gc::z
{
uint64_t ZAttachedArrayForForwarding::object_size() const noexcept
{
    return align_up(ZForwarding::get_size(), ZForwardingEntry::get_size());
}

ZForwardingEntry ZAttachedArrayForForwarding::get(ZForwarding obj) const noexcept
{
    return obj.address() + object_size();
}

uint64_t ZAttachedArrayForForwarding::align_up(uint64_t size, uint64_t alignment) noexcept
{
    uint64_t mask = alignment - 1;
    uint64_t adjusted = size + mask;
    return adjusted & ~mask;
}

void ZAttachedArrayForForwarding::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZAttachedArrayForForwarding");

    length_offset = *type->get_field_offset("_length");
}
} // namespace hotspot::gc::z