#pragma once

#include "shared/collectedHeap.hpp"

namespace hotspot::gc::parallel
{
class ParallelScavengeHeap : public shared::CollectedHeap
{
  public:
    ParallelScavengeHeap(uint64_t addr) noexcept : shared::CollectedHeap(addr) {}

    shared::CollectedHeapName kind() const noexcept override { return shared::CollectedHeapName::Parallel; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t young_gen_offset_;
    static inline uint64_t old_gen_offset_;
};
} // namespace hotspot::gc::parallel