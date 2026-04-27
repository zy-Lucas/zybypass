#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::memory
{
class HeapBlock : public runtime::JvmObject
{
  public:
    HeapBlock(uint64_t addr) : runtime::JvmObject(addr) {}

    uint64_t get_length() const noexcept { return get_header().get_length(); }

    bool is_free() const noexcept { return get_header().is_free(); }

    uint64_t get_allocated_space() const noexcept { return address() + heap_block_size; }

    struct Header : runtime::JvmObject
    {
        Header(uint64_t addr) : runtime::JvmObject(addr) {}

        uint64_t get_length() const noexcept { return read_field<uint64_t>(length_offset); }

        bool is_free() const noexcept { return !read_field<bool>(used_offset); }
    };

  private:
    Header get_header() const noexcept { return address() + header_offset; }

    DECLARE_STATIC_INIT

    static inline uint64_t header_offset;

    static inline uint64_t heap_block_size;

    static inline uint64_t length_offset;
    static inline uint64_t used_offset;
};
} // namespace hotspot::memory