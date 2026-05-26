#pragma once

#include "zAddress.hpp"
#include "zHeap.hpp"

namespace hotspot::gc::z
{
class ZCollectedHeap;

class ZBarrier
{
  public:
    static uint64_t weak_barrier(uint64_t o);

  private:
    static bool is_weak_good_or_null_fast_path(uint64_t addr) noexcept { return ZAddress::is_weak_good_or_null(addr); }

    static uint64_t weak_load_barrier_on_oop_slow_path(uint64_t addr);

    static ZHeap zheap();

    static bool during_relocate() noexcept { return ZGlobals::z_global_phase() == ZGlobals::z_phase_relocate_; }

    static uint64_t relocate(uint64_t addr) { return zheap().relocate_object(addr); }
    static uint64_t remap(uint64_t addr) { return zheap().remap_object(addr); }
    static uint64_t relocate_or_remap(uint64_t addr) { return during_relocate() ? relocate(addr) : remap(addr); }
};
} // namespace hotspot::gc::z