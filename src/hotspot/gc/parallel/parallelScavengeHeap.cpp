#include "parallelScavengeHeap.hpp"

namespace hotspot::gc::parallel
{
void ParallelScavengeHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ParallelScavengeHeap");

    young_gen_offset_ = *type->field_offset("_young_gen");
    old_gen_offset_ = *type->field_offset("_old_gen");
}
} // namespace hotspot::gc::parallel