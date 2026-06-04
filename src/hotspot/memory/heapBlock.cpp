#include "heapBlock.hpp"

namespace hotspot::memory
{
void HeapBlock::initialize()
{
    utils::FieldResolver r{"HeapBlock"};

    r.field_offset("_header", header_offset_);
    
    r.type_size(heap_block_size_);

    utils::FieldResolver r_h{"HeapBlock::Header"};

    r_h.field_offset("_length", length_offset_);
    r_h.field_offset("_used", used_offset_);
}
} // namespace hotspot::memory