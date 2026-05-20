#include "zGlobalsForVMStructs.hpp"

namespace hotspot::gc::z
{
uint32_t ZGlobalsForVMStructs::z_global_phase() const noexcept
{
    return runtime::Jvm::read<uint32_t>(read_field<uint64_t>(z_global_phase_offset_));
}

uint32_t ZGlobalsForVMStructs::z_global_seq_num() const noexcept
{
    return runtime::Jvm::read<uint32_t>(read_field<uint64_t>(z_global_seq_num_offset_));
}

uint64_t ZGlobalsForVMStructs::z_address_offset_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_offset_mask_offset_));
}

uint64_t ZGlobalsForVMStructs::z_address_metadata_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_metadata_mask_offset_));
}

uint64_t ZGlobalsForVMStructs::z_address_metadata_finalizable() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_metadata_finalizable_offset_));
}

uint64_t ZGlobalsForVMStructs::z_address_good_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_good_mask_offset_));
}

uint64_t ZGlobalsForVMStructs::z_address_bad_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_bad_mask_offset_));
}

uint64_t ZGlobalsForVMStructs::z_address_weak_bad_mask() const noexcept
{
    return runtime::Jvm::read<uint64_t>(read_field<uint64_t>(z_address_weak_bad_mask_offset_));
}

int32_t ZGlobalsForVMStructs::z_object_alignment_small_shift() const noexcept
{
    return runtime::Jvm::read<int32_t>(read_field<uint64_t>(z_object_alignment_small_shift_offset_));
}

int32_t ZGlobalsForVMStructs::z_object_alignment_small() const noexcept
{
    return runtime::Jvm::read<int32_t>(read_field<uint64_t>(z_object_alignment_small_offset_));
}

void ZGlobalsForVMStructs::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZGlobalsForVMStructs");

    z_global_phase_offset_ = *type->field_offset("_ZGlobalPhase");
    z_global_seq_num_offset_ = *type->field_offset("_ZGlobalSeqNum");
    z_address_offset_mask_offset_ = *type->field_offset("_ZAddressOffsetMask");
    z_address_metadata_mask_offset_ = *type->field_offset("_ZAddressMetadataMask");
    z_address_metadata_finalizable_offset_ = *type->field_offset("_ZAddressMetadataFinalizable");
    z_address_good_mask_offset_ = *type->field_offset("_ZAddressGoodMask");
    z_address_bad_mask_offset_ = *type->field_offset("_ZAddressBadMask");
    z_address_weak_bad_mask_offset_ = *type->field_offset("_ZAddressWeakBadMask");
    z_object_alignment_small_shift_offset_ = *type->field_offset("_ZObjectAlignmentSmallShift");
    z_object_alignment_small_offset_ = *type->field_offset("_ZObjectAlignmentSmall");
}
} // namespace hotspot::gc::z