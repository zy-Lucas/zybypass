#pragma once

#include "../shared/genCollectedHeap.hpp"

namespace hotspot::gc::serial
{
class SerialHeap : public shared::GenCollectedHeap
{
  public:
    SerialHeap(uint64_t addr) noexcept : shared::GenCollectedHeap(addr) {}

    shared::CollectedHeapName kind() { return shared::CollectedHeapName::Serial; }
};
} // namespace hotspot::gc::serial