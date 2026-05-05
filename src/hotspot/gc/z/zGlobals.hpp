#pragma once

#include "zGlobalsForVMStructs.hpp"

namespace hotspot::gc::z
{
class ZGlobals
{
  public:
    static uint32_t z_global_phase() noexcept { return instance().z_global_phase(); }
    static uint32_t z_global_seq_num() noexcept { return instance().z_global_seq_num(); }
    static uint64_t z_address_offset_mask() noexcept { return instance().z_address_offset_mask(); }
    static uint64_t z_address_metadata_mask() noexcept { return instance().z_address_metadata_mask(); }
    static uint64_t z_address_metadata_finalizable() noexcept { return instance().z_address_metadata_finalizable(); }
    static uint64_t z_address_good_mask() noexcept { return instance().z_address_good_mask(); }
    static uint64_t z_address_bad_mask() noexcept { return instance().z_address_bad_mask(); }
    static uint64_t z_address_weak_bad_mask() noexcept { return instance().z_address_weak_bad_mask(); }
    static int32_t z_object_alignment_small_shift() noexcept { return instance().z_object_alignment_small_shift(); }
    static int32_t z_object_alignment_small() noexcept { return instance().z_object_alignment_small(); }

    static inline uint32_t z_phase_relocate;

    static inline uint8_t z_page_type_small;
    static inline uint8_t z_page_type_medium;
    static inline uint8_t z_page_type_large;

    static inline uint64_t z_granule_size_shift;

    static inline uint64_t z_page_size_small_shift;
    static inline uint64_t z_page_size_medium_shift;

    static inline int32_t z_object_alignment_medium_shift;
    static inline int32_t z_object_alignment_large_shift;

    static inline uint64_t z_address_offset_shift;

    static inline uint64_t z_address_offset_bits;
    static inline uint64_t z_address_offset_max;

  private:
    static ZGlobalsForVMStructs instance() noexcept { return runtime::Jvm::read<uint64_t>(instance_offset); }

    DECLARE_STATIC_INIT

    static inline uint64_t instance_offset;
};
} // namespace hotspot::gc::z