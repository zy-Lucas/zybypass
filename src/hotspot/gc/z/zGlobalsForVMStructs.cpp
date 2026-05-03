#include "zGlobalsForVMStructs.hpp"

namespace hotspot::gc::z
{
uint32_t ZGlobalsForVMStructs::z_global_phase() const noexcept
{
    return runtime::Jvm::read<uint32_t>(read_field<uint64_t>(z_global_phase_offset));
}

uint32_t ZGlobalsForVMStructs::z_global_seq_num() const noexcept
{
    return runtime::Jvm::read<uint32_t>(read_field<uint64_t>(z_global_seq_num_offset));
}

uint64_t ZGlobalsForVMStructs::z_address_offset_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_offset_mask_offset));
}

uint64_t ZGlobalsForVMStructs::z_address_metadata_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_metadata_mask_offset));
}

uint64_t ZGlobalsForVMStructs::z_address_metadata_finalizable() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_metadata_finalizable_offset));
}

uint64_t ZGlobalsForVMStructs::z_address_good_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_good_mask_offset));
}

uint64_t ZGlobalsForVMStructs::z_address_bad_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_bad_mask_offset));
}

uint64_t ZGlobalsForVMStructs::z_address_weak_bad_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_weak_bad_mask_offset));
}

int32_t ZGlobalsForVMStructs::z_object_alignment_small_shift() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_object_alignment_small_shift_offset));
}

int32_t ZGlobalsForVMStructs::z_object_alignment_small() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_object_alignment_small_offset));
}

void ZGlobalsForVMStructs::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZGlobalsForVMStructs");

    z_global_phase_offset = *type->get_field_offset("_ZGlobalPhase");
    z_global_seq_num_offset = *type->get_field_offset("_ZGlobalSeqNum");
    z_address_offset_mask_offset = *type->get_field_offset("_ZAddressOffsetMask");
    z_address_metadata_mask_offset = *type->get_field_offset("_ZAddressMetadataMask");
    z_address_metadata_finalizable_offset = *type->get_field_offset("_ZAddressMetadataFinalizable");
    z_address_good_mask_offset = *type->get_field_offset("_ZAddressGoodMask");
    z_address_bad_mask_offset = *type->get_field_offset("_ZAddressBadMask");
    z_address_weak_bad_mask_offset = *type->get_field_offset("_ZAddressWeakBadMask");
    z_object_alignment_small_shift_offset = *type->get_field_offset("_ZObjectAlignmentSmallShift");
    z_object_alignment_small_offset = *type->get_field_offset("_ZObjectAlignmentSmall");
}
} // namespace hotspot::gc::z