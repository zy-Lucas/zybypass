#include "parallelScavengeHeap.hpp"

namespace hotspot::gc::parallel
{
void ParallelScavengeHeap::initialize()
{
    utils::FieldResolver r{"ParallelScavengeHeap"};
    
    r.field_offset("_young_gen", young_gen_offset_);
    r.field_offset("_old_gen", old_gen_offset_);
}
} // namespace hotspot::gc::parallel