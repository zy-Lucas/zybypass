#include "shenandoahHeap.hpp"

namespace hotspot::gc::shenandoah
{
void ShenandoahHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ShenandoahHeap");
    types::Type *region_ptr_type = runtime::Jvm::lookup_type("ShenandoahHeapRegion*");

    used_offset_ = *type->field_offset("_used");
    committed_offset_ = *type->field_offset("_committed");
    regions_offset_ = *type->field_offset("_regions");
    num_regions_offset_ = *type->field_offset("_num_regions");
    log_min_obj_alignment_in_bytes_offset_ = *type->field_offset("_log_min_obj_alignment_in_bytes");

    region_ptr_size_ = region_ptr_type->size();
}
} // namespace hotspot::gc::shenandoah