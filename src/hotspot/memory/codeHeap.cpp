#include "codeHeap.hpp"

namespace hotspot::memory
{
CodeHeap::CodeHeap(uint64_t addr)
    : runtime::JvmObject(addr), memory(address() + memory_offset), segment_map(address() + segmap_offset),
      log2_segment_size(read_field<int32_t>(log2_segment_size_offset))
{
}

uint64_t CodeHeap::find_start(uint64_t p) const noexcept
{
    if (!contains(p))
        return 0;
    HeapBlock h{block_start(p)};
    if (!h || h.is_free())
        return 0;
    return h.get_allocated_space();
}

uint64_t CodeHeap::block_base(uint64_t p) const noexcept
{
    uint64_t i = segment_for(p);
    uint64_t b = segment_map.low();
    if (runtime::Jvm::read<uint8_t>(b + i) == 0xFF)
        return 0;
    while (runtime::Jvm::read<uint8_t>(b + i))
        i -= runtime::Jvm::read<uint8_t>(b + i);
    return begin() + (i << log2_segment_size);
}

uint64_t CodeHeap::next_block(uint64_t p) const noexcept
{
    if (uint64_t base = block_base(p))
        return base + HeapBlock(base).get_length() * (1ull << log2_segment_size);
    return 0;
}

void CodeHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CodeHeap");

    memory_offset = *type->get_field_offset("_memory");
    segmap_offset = *type->get_field_offset("_segmap");
    log2_segment_size_offset = *type->get_field_offset("_log2_segment_size");
}
} // namespace hotspot::memory