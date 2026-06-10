#pragma once

#include "counters.hpp"
#include "iostream"

namespace hotspot::memory::metaspace
{
template <uint64_t smallest_word_size, int32_t num_lists> class BinListImpl
{
  public:
    BinListImpl() noexcept
    {
        for (uint32_t i = 0; i < num_lists; i++)
            blocks_[i] = 0;
    }

    void add_block(uint64_t *p, uint64_t word_size) noexcept
    {
        if (word_size < MinWordSize || word_size > MaxWordSize)
            return;
        int32_t index = index_for_word_size(word_size);
        Block *old_head = blocks_[index];
        Block *new_head = new (p) Block(old_head, word_size);
        blocks_[index] = new_head;
        counter_.add(word_size);
    }

    uint64_t *remove_block(uint64_t word_size, uint64_t *p_real_word_size) noexcept
    {
        *p_real_word_size = 0;
        if (word_size < MinWordSize || word_size > MaxWordSize)
            return nullptr;
        int32_t index = index_for_next_non_empty_list(index_for_word_size(word_size));
        if (index == -1)
            return nullptr;
        Block *b = blocks_[index];
        blocks_[index] = b->next_;
        counter_.sub(b->word_size_);
        *p_real_word_size = b->word_size_;
        return (uint64_t *)b;
    }

    uint32_t count() const noexcept { return counter_.count(); }

    uint64_t total_size() const noexcept { return counter_.total_size(); }

    bool is_empty() const noexcept { return !count(); }

    // void print_all_blocks() const
    // {
    //     std::cout << "[BinList] dump begin (count=" << counter_.count() << ", total_size=" << counter_.total_size()
    //               << ")" << std::endl;
    //     for (int32_t i = 0; i < num_lists; ++i)
    //     {
    //         if (Block *b = blocks_[i]; b != nullptr)
    //         {
    //             std::cout << "  Bin[" << word_size_for_index(i) << "]:" << std::endl;
    //             for (Block *curr = b; curr != nullptr; curr = curr->next_)
    //             {
    //                 std::cout << "    Block@" << curr << " word_size=" << curr->word_size_ << " next=" << curr->next_
    //                           << std::endl;
    //             }
    //         }
    //     }
    //     std::cout << "[BinList] dump end" << std::endl;
    // }

    static constexpr uint64_t MinWordSize = smallest_word_size;
    static constexpr uint64_t MaxWordSize = MinWordSize + num_lists - 1;

  private:
    int32_t index_for_next_non_empty_list(int32_t index) noexcept
    {
        if (index < 0 || index >= num_lists)
            return -1;
        while (index < num_lists && !blocks_[index])
        {
            Block *b = blocks_[index];
            std::cout << "  Bin[" << word_size_for_index(index) << "]:" << std::endl;
            for (Block *curr = b; curr != nullptr; curr = curr->next_)
            {
                std::cout << "    Block@" << curr << " word_size=" << curr->word_size_ << " next=" << curr->next_
                          << std::endl;
            }
            index++;
        }
        return index == num_lists ? -1 : index;
    }

    static int32_t index_for_word_size(uint64_t word_size) noexcept { return word_size - MinWordSize; }
    static uint64_t word_size_for_index(int32_t index) noexcept { return MinWordSize + index; }

    struct Block
    {
        Block *next_;
        uint64_t word_size_;
    };

    Block *blocks_[num_lists];

    MemRangeCounter counter_;
};

typedef BinListImpl<2, 32> BinList32;
} // namespace hotspot::memory::metaspace