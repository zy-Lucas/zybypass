#include "freeBlocks.hpp"

namespace hotspot::memory::metaspace
{
void FreeBlocks::add_block(uint64_t *p, uint64_t word_size) noexcept
{
    if (word_size < BinList32::MinWordSize)
        return;
    if (word_size > BinList32::MaxWordSize)
    {
        tree()->add_block(p, word_size);
        return;
    }
    small_blocks()->add_block(p, word_size);
}

uint64_t *FreeBlocks::remove_block(uint64_t requested_word_size) noexcept
{
    if (requested_word_size < BinList32::MinWordSize)
        return nullptr;
    uint64_t real_size = 0;
    uint64_t *p = nullptr;
    if (requested_word_size > BinList32::MaxWordSize)
        p = tree()->remove_block(requested_word_size, &real_size);
    else
        p = small_blocks()->remove_block(requested_word_size, &real_size);
    if (p)
        if (uint64_t waste = real_size - requested_word_size; waste > BinList32::MinWordSize)
            add_block(p + requested_word_size, waste);
    return p;
}

void FreeBlocks::initialize()
{
    small_blocks_offset_ = 0;
    tree_offset_ = sizeof(BinList32);
}
} // namespace hotspot::memory::metaspace