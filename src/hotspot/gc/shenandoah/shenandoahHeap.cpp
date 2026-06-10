#include "shenandoahHeap.hpp"

namespace hotspot::gc::shenandoah
{
void ShenandoahHeap::initialize()
{
    utils::FieldResolver r{"ShenandoahHeap"};

    r.field_offset("_used", used_offset_);
    r.field_offset("_committed", committed_offset_);
    r.field_offset("_regions", regions_offset_);
    r.field_offset("_num_regions", num_regions_offset_);
    r.field_offset("_log_min_obj_alignment_in_bytes", log_min_obj_alignment_in_bytes_offset_);

    utils::FieldResolver r_region_p{"ShenandoahHeapRegion*"};

    r_region_p.type_size(region_ptr_size_);
}
} // namespace hotspot::gc::shenandoah