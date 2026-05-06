#include "zForwardingTable.hpp"

namespace hotspot::gc::z
{
void ZForwardingTable::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZForwardingTable");

    map_offset = *type->get_field_offset("_map");
}
} // namespace hotspot::gc::z