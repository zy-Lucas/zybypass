#pragma once

#include "../shared/collectedHeap.hpp"

namespace hotspot::gc::epsilon
{
class EpsilonHeap : public shared::CollectedHeap
{
  public:
    EpsilonHeap(uint64_t addr) noexcept : shared::CollectedHeap(addr) {}

    shared::CollectedHeapName kind() const noexcept override { return shared::CollectedHeapName::Epsilon; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t space_offset_;
    static inline uint64_t virtual_space_offset_;
};
} // namespace hotspot::gc::epsilon