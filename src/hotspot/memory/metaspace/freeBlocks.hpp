#pragma once

#include "memory/metaspace/binList.hpp"
#include "memory/metaspace/blockTree.hpp"

namespace hotspot::memory::metaspace
{
class FreeBlocks : public runtime::JvmObject
{
  public:
    FreeBlocks(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    BinList32 *small_blocks() const noexcept { return (BinList32 *)(address() + small_blocks_offset_); }
    BlockTree *tree() const noexcept { return (BlockTree *)(address() + tree_offset_); }

    void add_block(uint64_t *p, uint64_t word_size) noexcept;
    uint64_t *remove_block(uint64_t requested_word_size) noexcept;

    uint32_t count() const noexcept { return small_blocks()->count() + tree()->count(); }

    uint64_t total_size() const noexcept { return small_blocks()->total_size() + tree()->total_size(); }

    bool is_empty() const noexcept { return small_blocks()->is_empty() && tree()->is_empty(); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t small_blocks_offset_;
    static inline uint64_t tree_offset_;
};
} // namespace hotspot::memory::metaspace