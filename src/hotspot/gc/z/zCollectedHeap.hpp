#pragma once

#include "shared/collectedHeap.hpp"
#include "zBarrier.hpp"
#include "zHeap.hpp"

namespace hotspot::gc::z
{
class ZCollectedHeap : public shared::CollectedHeap
{
  public:
    ZCollectedHeap(uint64_t addr) noexcept : shared::CollectedHeap(addr) {}

    ZHeap heap() const noexcept { return address() + heap_offset_; }

    shared::CollectedHeapName kind() const noexcept override { return shared::CollectedHeapName::Z; }

    debugger::OopHandle oop_load_at(debugger::OopHandle handle, uint64_t offset) const override;
    debugger::OopHandle oop_load_in_native(uint64_t addr) const override;

  private:
    debugger::OopHandle oop_load_barrier(uint64_t oop_address) const { return ZBarrier::weak_barrier(oop_address); }

    DECLARE_STATIC_INIT

    static inline uint64_t heap_offset_;
};
} // namespace hotspot::gc::z