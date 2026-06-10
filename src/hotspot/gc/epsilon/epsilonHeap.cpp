#include "epsilonHeap.hpp"

namespace hotspot::gc::epsilon
{
void EpsilonHeap::initialize()
{
    utils::FieldResolver r{"EpsilonHeap"};

    r.field_offset("_space", space_offset_);
    r.field_offset("_virtual_space", virtual_space_offset_);
}
} // namespace hotspot::gc::epsilon