#include "virtualSpace.hpp"

namespace hotspot::memory
{
void VirtualSpace::initialize()
{
    utils::FieldResolver r{"VirtualSpace"};

    r.field_offset("_low", low_offset_);
    r.field_offset("_high", high_offset_);
    r.field_offset("_low_boundary", low_boundary_offset_);
    r.field_offset("_high_boundary", high_boundary_offset_);
}
} // namespace hotspot::memory