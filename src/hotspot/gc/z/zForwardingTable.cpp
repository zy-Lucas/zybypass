#include "zForwardingTable.hpp"

namespace hotspot::gc::z
{
void ZForwardingTable::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZForwardingTable");

    map_offset_ = *type->field_offset("_map");
}
} // namespace hotspot::gc::z