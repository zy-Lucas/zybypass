#pragma once

#include "../shared/collectedHeap.hpp"

namespace hotspot::gc::shenandoah
{
class ShenandoahHeap : public shared::CollectedHeap
{
  public:
    ShenandoahHeap(uint64_t addr) : shared::CollectedHeap(addr) {}

    shared::CollectedHeapName kind() const noexcept override { return shared::CollectedHeapName::Shenandoah; }

    uint64_t used() const noexcept override { return read_field<uint64_t>(used_offset); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t used_offset;
    static inline uint64_t committed_offset;
    static inline uint64_t regions_offset;
    static inline uint64_t num_regions_offset;
    static inline uint64_t log_min_obj_alignment_in_bytes_offset;

    static inline uint64_t region_ptr_size;
};
} // namespace hotspot::gc::shenandoah