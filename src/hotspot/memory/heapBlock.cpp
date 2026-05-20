#include "heapBlock.hpp"

namespace hotspot::memory
{
void HeapBlock::initialize()
{
    types::Type *type_heap_block = runtime::Jvm::lookup_type("HeapBlock");
    types::Type *type_header = runtime::Jvm::lookup_type("HeapBlock::Header");

    header_offset_ = *type_heap_block->field_offset("_header");

    heap_block_size_ = type_heap_block->size();

    length_offset_ = *type_header->field_offset("_length");
    used_offset_ = *type_header->field_offset("_used");
}
} // namespace hotspot::memory