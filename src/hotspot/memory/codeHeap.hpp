#pragma once

#include "../code/codeBlob.hpp"
#include "../runtime/jvmObject.hpp"
#include "heapBlock.hpp"
#include "virtualSpace.hpp"

namespace hotspot::memory
{
class CodeHeap : public runtime::JvmObject
{
  public:
    CodeHeap(uint64_t addr);

    uint64_t begin() const noexcept { return memory.low(); }
    uint64_t end() const noexcept { return memory.high(); }

    std::string_view get_name_view() const noexcept { return read_string_field(log2_segment_size_offset + 56); }
    std::string get_name() const noexcept { return std::string{get_name_view()}; }

    int32_t get_code_blob_type() const noexcept { return read_field<int32_t>(log2_segment_size_offset + 64); }

    bool contains(uint64_t p) const noexcept { return begin() <= p && end() > p; }

    uint64_t find_start(uint64_t p) const noexcept;

    void iterate(auto &&visitor);

  private:
    uint64_t segment_for(uint64_t p) const noexcept { return (p - begin()) >> log2_segment_size; }

    HeapBlock block_start(uint64_t p) const noexcept { return block_base(p); }

    uint64_t block_base(uint64_t p) const noexcept;
    uint64_t next_block(uint64_t p) const noexcept;

    VirtualSpace memory;
    VirtualSpace segment_map;
    int32_t log2_segment_size;

    DECLARE_STATIC_INIT

    static inline uint64_t memory_offset;
    static inline uint64_t segmap_offset;
    static inline uint64_t log2_segment_size_offset;
};

inline void CodeHeap::iterate(auto &&visitor)
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
            if (code::CodeBlob blob{block.get_allocated_space()}; blob)
            {
                visitor(blob);
                if (blob == last_blob)
                    throw std::runtime_error("saw same blob twice");
                last_blob = blob;
            }
        uint64_t next = base + (block.get_length() << log2_segment_size);
        if (next != 0 && next < ptr)
            throw std::runtime_error("pointer moved backwards");
        ptr = next;
    }
}
} // namespace hotspot::memory