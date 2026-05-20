#include "zAttachedArrayForForwarding.hpp"
#include "zForwarding.hpp"

namespace hotspot::gc::z
{
uint64_t ZAttachedArrayForForwarding::object_size() const noexcept
{
    return align_up(ZForwarding::size(), ZForwardingEntry::size());
}

ZForwardingEntry ZAttachedArrayForForwarding::entry(ZForwarding obj) const noexcept
{
    return obj.address() + object_size();
}

uint64_t ZAttachedArrayForForwarding::align_up(uint64_t size, uint64_t alignment) noexcept
{
    uint64_t mask = alignment - 1;
    return size + mask & ~mask;
}

void ZAttachedArrayForForwarding::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZAttachedArrayForForwarding");

    length_offset_ = *type->field_offset("_length");
}
} // namespace hotspot::gc::z