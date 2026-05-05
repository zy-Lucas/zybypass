#include "zGranuleMapForForwarding.hpp"

namespace hotspot::gc::z
{
void ZGranuleMapForForwarding::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZGranuleMapForForwarding");

    map_offset = *type->get_field_offset("_map");
}
} // namespace hotspot::gc::z