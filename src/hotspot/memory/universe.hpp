#pragma once

#include "../gc/shared/collectedHeap.hpp"
#include "../runtime/jvmObject.hpp"

namespace hotspot::memory
{
class Universe
{
  public:
    static std::unique_ptr<gc::shared::CollectedHeap> heap();

    static bool is_in(uint64_t p) { return heap()->is_in(p); }
    static bool is_in_reserved(uint64_t p) { return heap()->is_in_reserved(p); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t collected_heap_offset;
};
} // namespace hotspot::memory