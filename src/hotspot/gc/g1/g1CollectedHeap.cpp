#include "g1CollectedHeap.hpp"

namespace hotspot::gc::g1
{
void G1CollectedHeap::initialize()
{
    utils::FieldResolver r{"G1CollectedHeap"};
    
    r.field_offset("_old_set", old_set_offset_);
    r.field_offset("_archive_set", archive_set_offset_);
    r.field_offset("_humongous_set", humongous_set_offset_);
    r.field_offset("_hrm", hrm_offset_);
    r.field_offset("_summary_bytes_used", summary_bytes_used_offset_);
    r.field_offset("_g1mm", g1mm_offset_);
}
}