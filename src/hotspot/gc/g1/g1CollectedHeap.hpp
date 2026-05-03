#pragma once

#include "../shared/collectedHeap.hpp"

namespace hotspot::gc::g1
{
class G1CollectedHeap : public shared::CollectedHeap
{
  public:
    G1CollectedHeap(uint64_t addr) : shared::CollectedHeap(addr) {}

    shared::CollectedHeapName kind() const noexcept override { return shared::CollectedHeapName::G1; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t old_set_offset;
    static inline uint64_t archive_set_offset;
    static inline uint64_t humongous_set_offset;
    static inline uint64_t hrm_offset;
    static inline uint64_t summary_bytes_used_offset;
    static inline uint64_t g1mm_offset;
};
} // namespace hotspot::gc::g1