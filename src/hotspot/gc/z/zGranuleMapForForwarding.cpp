#include "zGranuleMapForForwarding.hpp"

namespace hotspot::gc::z
{
uint64_t ZGranuleMapForForwarding::forwarding_at(uint64_t index) const noexcept
{
    return runtime::Jvm::read<uint64_t>(map() + index * sizeof(void *));
}

void ZGranuleMapForForwarding::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZGranuleMapForForwarding");

    map_offset_ = *type->field_offset("_map");
}
} // namespace hotspot::gc::z