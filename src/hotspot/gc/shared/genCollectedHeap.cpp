#include "genCollectedHeap.hpp"

namespace hotspot::gc::shared
{
void GenCollectedHeap::initialize()
{
    utils::FieldResolver r{"GenCollectedHeap"};
    
    r.field_offset("_young_gen", young_gen_offset_);
    r.field_offset("_old_gen", old_gen_offset_);
    r.field_offset("_young_gen_spec", young_gen_spec_offset_);
    r.field_offset("_old_gen_spec", old_gen_spec_offset_);
}
} // namespace hotspot::gc::shared