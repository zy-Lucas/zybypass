#include "zForwardingTable.hpp"

namespace hotspot::gc::z
{
void ZForwardingTable::initialize()
{
    utils::FieldResolver r{"ZForwardingTable"};

    r.field_offset("_map", map_offset_);
}
} // namespace hotspot::gc::z