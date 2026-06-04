#include "zBarrier.hpp"
#include "memory/universe.hpp"
#include "zCollectedHeap.hpp"

namespace hotspot::gc::z
{
uint64_t ZBarrier::weak_barrier(uint64_t o)
{
    if (is_weak_good_or_null_fast_path(o))
        return ZAddress::good_or_null(o);
    return weak_load_barrier_on_oop_slow_path(o);
}

uint64_t ZBarrier::weak_load_barrier_on_oop_slow_path(uint64_t addr)
{
    return ZAddress::is_weak_good(addr) ? ZAddress::good(addr) : relocate_or_remap(addr);
}

ZHeap ZBarrier::zheap() { return ((ZCollectedHeap *)memory::Universe::heap())->heap(); }
} // namespace hotspot::gc::z