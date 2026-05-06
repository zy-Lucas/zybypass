#include "shenandoahHeap.hpp"

namespace hotspot::gc::shenandoah
{
void ShenandoahHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ShenandoahHeap");
    types::Type *region_ptr_type = runtime::Jvm::lookup_type("ShenandoahHeapRegion*");

    used_offset = *type->get_field_offset("_used");
    committed_offset = *type->get_field_offset("_committed");
    regions_offset = *type->get_field_offset("_regions");
    num_regions_offset = *type->get_field_offset("_num_regions");
    log_min_obj_alignment_in_bytes_offset = *type->get_field_offset("_log_min_obj_alignment_in_bytes");

    region_ptr_size = region_ptr_type->get_size();
}
} // namespace hotspot::gc::shenandoah