#include "zGranuleMapForForwarding.hpp"

namespace hotspot::gc::z
{
uint64_t ZGranuleMapForForwarding::forwarding_at(uint64_t index) const noexcept
{
    return runtime::Jvm::read<uint64_t>(map() + index * sizeof(void *));
}

void ZGranuleMapForForwarding::initialize()
{
    utils::FieldResolver r{"ZGranuleMapForForwarding"};
    
    r.field_offset("_map", map_offset_);
}
} // namespace hotspot::gc::z