#include "g1CollectedHeap.hpp"

namespace hotspot::gc::g1
{
void G1CollectedHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("G1CollectedHeap");

    old_set_offset_ = *type->field_offset("_old_set");
    archive_set_offset_ = *type->field_offset("_archive_set");
    humongous_set_offset_ = *type->field_offset("_humongous_set");
    hrm_offset_ = *type->field_offset("_hrm");
    summary_bytes_used_offset_ = *type->field_offset("_summary_bytes_used");
    g1mm_offset_ = *type->field_offset("_g1mm");
}
}