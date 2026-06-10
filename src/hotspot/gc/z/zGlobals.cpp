#include "zGlobals.hpp"

namespace hotspot::gc::z
{
void ZGlobals::initialize()
{
    utils::FieldResolver r{"ZGlobalsForVMStructs"};

    r.field_offset("_instance_p", instance_offset);

    utils::constants::int_const("ZPhaseRelocate", z_phase_relocate_);

    utils::constants::int_const("ZPageTypeSmall", z_page_type_small_);
    utils::constants::int_const("ZPageTypeMedium", z_page_type_medium_);
    utils::constants::int_const("ZPageTypeLarge", z_page_type_large_);

    utils::constants::long_const("ZGranuleSizeShift", z_granule_size_shift_);
    
    utils::constants::long_const("ZPageSizeSmallShift", z_page_size_small_shift_);
    utils::constants::long_const("ZPageSizeMediumShift", z_page_size_medium_shift_);

    utils::constants::int_const("ZObjectAlignmentMediumShift", z_object_alignment_medium_shift_);
    utils::constants::int_const("ZObjectAlignmentLargeShift", z_object_alignment_large_shift_);

    utils::constants::long_const("ZAddressOffsetShift", z_address_offset_shift_);

    utils::constants::long_const("ZAddressOffsetBits", z_address_offset_bits_);
    utils::constants::long_const("ZAddressOffsetMax", z_address_offset_max_);
}
} // namespace hotspot::gc::z