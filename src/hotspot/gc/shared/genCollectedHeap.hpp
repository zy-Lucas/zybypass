#pragma once

#include "collectedHeap.hpp"

namespace hotspot::gc::shared
{
class GenCollectedHeap : public CollectedHeap
{
  public:
    GenCollectedHeap(uint64_t addr) : CollectedHeap(addr) {}

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t young_gen_offset;
    static inline uint64_t old_gen_offset;
    static inline uint64_t young_gen_spec_offset;
    static inline uint64_t old_gen_spec_offset;
};
} // namespace hotspot::gc::shared