#include "zGlobals.hpp"

namespace hotspot::gc::z
{
void ZGlobals::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZGlobalsForVMStructs");

    instance_offset = *type->field_offset("_instance_p");

    z_phase_relocate_ = *runtime::Jvm::lookup_int_constant("ZPhaseRelocate");

    z_page_type_small_ = *runtime::Jvm::lookup_int_constant("ZPageTypeSmall");
    z_page_type_medium_ = *runtime::Jvm::lookup_int_constant("ZPageTypeMedium");
    z_page_type_large_ = *runtime::Jvm::lookup_int_constant("ZPageTypeLarge");

    z_granule_size_shift_ = *runtime::Jvm::lookup_long_constant("ZGranuleSizeShift");

    z_page_size_small_shift_ = *runtime::Jvm::lookup_long_constant("ZPageSizeSmallShift");
    z_page_size_medium_shift_ = *runtime::Jvm::lookup_long_constant("ZPageSizeMediumShift");

    z_object_alignment_medium_shift_ = *runtime::Jvm::lookup_int_constant("ZObjectAlignmentMediumShift");
    z_object_alignment_large_shift_ = *runtime::Jvm::lookup_int_constant("ZObjectAlignmentLargeShift");

    z_address_offset_shift_ = *runtime::Jvm::lookup_long_constant("ZAddressOffsetShift");

    z_address_offset_bits_ = *runtime::Jvm::lookup_long_constant("ZAddressOffsetBits");
    z_address_offset_max_ = *runtime::Jvm::lookup_long_constant("ZAddressOffsetMax");
}
} // namespace hotspot::gc::z