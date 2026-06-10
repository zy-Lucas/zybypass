#pragma once

#include "code/codeBlob.hpp"
#include "heapBlock.hpp"
#include "virtualSpace.hpp"

namespace hotspot::memory
{
class CodeHeap : public runtime::JvmObject
{
  public:
    CodeHeap(uint64_t addr) noexcept;

    uint64_t begin() const noexcept { return memory_.low(); }
    uint64_t end() const noexcept { return memory_.high(); }

    std::string_view name_view() const noexcept { return read_string_field(log2_segment_size_offset_ + 56); }
    std::string name() const { return std::string{name_view()}; }

    int32_t code_blob_type() const noexcept { return read_field<int32_t>(log2_segment_size_offset_ + 64); }

    bool contains(uint64_t p) const noexcept { return begin() <= p && end() > p; }

    uint64_t find_start(uint64_t p) const noexcept;

    void iterate(auto &&visitor) const;

  private:
    uint64_t segment_for(uint64_t p) const noexcept { return (p - begin()) >> log2_segment_size_; }

    HeapBlock block_start(uint64_t p) const noexcept { return block_base(p); }

    uint64_t block_base(uint64_t p) const noexcept;
    uint64_t next_block(uint64_t p) const noexcept;

    VirtualSpace memory_;
    VirtualSpace segment_map_;
    int32_t log2_segment_size_;

    DECLARE_STATIC_INIT

    static inline uint64_t memory_offset_;
    static inline uint64_t segmap_offset_;
    static inline uint64_t log2_segment_size_offset_;
};

void CodeHeap::iterate(auto &&visitor) const
{
    code::CodeBlob last_blob{0};
    uint64_t ptr = begin();
    uint64_t const heap_end = end();

    while (ptr && ptr < heap_end)
    {
        uint64_t base = block_base(ptr);
        if (!base)
            break;
        HeapBlock block{base};
        if (!block.is_free())
            if (code::CodeBlob blob{block.allocated_space()}; blob)
            {
                visitor(blob);
                if (blob == last_blob)
                    throw std::runtime_error("saw same blob twice");
                last_blob = blob;
            }
        uint64_t next = base + (block.length() << log2_segment_size_);
        if (next && next < ptr)
            throw std::runtime_error("pointer moved backwards");
        ptr = next;
    }
}
} // namespace hotspot::memory