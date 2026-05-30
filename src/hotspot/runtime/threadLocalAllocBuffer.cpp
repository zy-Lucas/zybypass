#include "threadLocalAllocBuffer.hpp"
#include "../gc/shared/collectedHeap.hpp"

namespace hotspot::runtime
{
uint64_t ThreadLocalAllocBuffer::allocate(uint64_t size) noexcept
{
    if (uint64_t obj = top(); end() - obj >= size)
    {
        set_top(obj + size);
        return obj;
    }
    return 0;
}

uint64_t ThreadLocalAllocBuffer::end_reserve() noexcept
{
    return std::max<uint64_t>(gc::shared::CollectedHeap::tlab_alloc_reserve(),
                              Jvm::reserve_for_allocation_prefetch() * Jvm::heap_word_size());
}

void ThreadLocalAllocBuffer::initialize()
{
    types::Type *type = Jvm::lookup_type("ThreadLocalAllocBuffer");

    start_offset_ = *type->field_offset("_start");
    top_offset_ = *type->field_offset("_top");
    end_offset_ = *type->field_offset("_end");
    allocation_end_offset_ = end_offset_ + sizeof(void *);
    desired_size_offset_ = *type->field_offset("_desired_size");
}
} // namespace hotspot::runtime