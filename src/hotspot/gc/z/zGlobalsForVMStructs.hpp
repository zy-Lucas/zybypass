#pragma once

#include "../../runtime/jvmObject.hpp"

namespace hotspot::gc::z
{
class ZGlobalsForVMStructs : public runtime::JvmObject
{
  public:
    ZGlobalsForVMStructs(uint64_t addr) : runtime::JvmObject(addr) {}

    uint32_t z_global_phase() const noexcept;
    uint32_t z_global_seq_num() const noexcept;
    uint64_t z_address_offset_mask() const noexcept;
    uint64_t z_address_metadata_mask() const noexcept;
    uint64_t z_address_metadata_finalizable() const noexcept;
    uint64_t z_address_good_mask() const noexcept;
    uint64_t z_address_bad_mask() const noexcept;
    uint64_t z_address_weak_bad_mask() const noexcept;
    int32_t z_object_alignment_small_shift() const noexcept;
    int32_t z_object_alignment_small() const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t z_global_phase_offset;
    static inline uint64_t z_global_seq_num_offset;
    static inline uint64_t z_address_offset_mask_offset;
    static inline uint64_t z_address_metadata_mask_offset;
    static inline uint64_t z_address_metadata_finalizable_offset;
    static inline uint64_t z_address_good_mask_offset;
    static inline uint64_t z_address_bad_mask_offset;
    static inline uint64_t z_address_weak_bad_mask_offset;
    static inline uint64_t z_object_alignment_small_shift_offset;
    static inline uint64_t z_object_alignment_small_offset;
};
} // namespace hotspot::gc::z