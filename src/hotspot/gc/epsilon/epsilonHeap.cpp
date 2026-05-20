#include "epsilonHeap.hpp"

namespace hotspot::gc::epsilon
{
void EpsilonHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("EpsilonHeap");

    space_offset_ = *type->field_offset("_space");
    virtual_space_offset_ = *type->field_offset("_virtual_space");
}
} // namespace hotspot::gc::epsilon