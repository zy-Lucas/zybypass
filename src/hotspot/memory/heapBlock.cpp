#include "heapBlock.hpp"

namespace hotspot::memory
{
void HeapBlock::initialize()
{
    types::Type *type_heap_block = runtime::Jvm::lookup_type("HeapBlock");
    types::Type *type_header = runtime::Jvm::lookup_type("HeapBlock::Header");

    header_offset = *type_heap_block->get_field_offset("_header");

    heap_block_size = type_heap_block->get_size();

    length_offset = *type_header->get_field_offset("_length");
    used_offset = *type_header->get_field_offset("_used");
}
} // namespace hotspot::memory