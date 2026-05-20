#include "codeHeap.hpp"

namespace hotspot::memory
{
CodeHeap::CodeHeap(uint64_t addr) noexcept
    : runtime::JvmObject(addr), memory_(address() + memory_offset_), segment_map_(address() + segmap_offset_),
      log2_segment_size_(read_field<int32_t>(log2_segment_size_offset_))
{
}

uint64_t CodeHeap::find_start(uint64_t p) const noexcept
{
    if (!contains(p))
        return 0;
    if (HeapBlock h{block_start(p)}; h && !h.is_free())
        return h.allocated_space();
    return 0;
}

uint64_t CodeHeap::block_base(uint64_t p) const noexcept
{
    uint64_t i = segment_for(p);
    uint64_t b = segment_map_.low();
    if (runtime::Jvm::read<uint8_t>(b + i) == 0xFF)
        return 0;
    while (runtime::Jvm::read<uint8_t>(b + i))
        i -= runtime::Jvm::read<uint8_t>(b + i);
    return begin() + (i << log2_segment_size_);
}

uint64_t CodeHeap::next_block(uint64_t p) const noexcept
{
    if (uint64_t base = block_base(p); base)
        return base + HeapBlock(base).length() * (1ull << log2_segment_size_);
    return 0;
}

void CodeHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CodeHeap");

    memory_offset_ = *type->field_offset("_memory");
    segmap_offset_ = *type->field_offset("_segmap");
    log2_segment_size_offset_ = *type->field_offset("_log2_segment_size");
}
} // namespace hotspot::memory