#include "virtualSpace.hpp"

namespace hotspot::memory
{
void VirtualSpace::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("VirtualSpace");

    low_offset_ = *type->field_offset("_low");
    high_offset_ = *type->field_offset("_high");
    low_boundary_offset_ = *type->field_offset("_low_boundary");
    high_boundary_offset_ = *type->field_offset("_high_boundary");
}
} // namespace hotspot::memory