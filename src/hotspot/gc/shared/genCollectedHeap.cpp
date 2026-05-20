#include "genCollectedHeap.hpp"

namespace hotspot::gc::shared
{
void GenCollectedHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("GenCollectedHeap");

    young_gen_offset_ = *type->field_offset("_young_gen");
    old_gen_offset_ = *type->field_offset("_old_gen");
    young_gen_spec_offset_ = *type->field_offset("_young_gen_spec");
    old_gen_spec_offset_ = *type->field_offset("_old_gen_spec");
}
} // namespace hotspot::gc::shared