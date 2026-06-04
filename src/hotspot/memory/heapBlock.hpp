#pragma once

#include "runtime/jvmObject.hpp"

namespace hotspot::memory
{
class HeapBlock : public runtime::JvmObject
{
  public:
    HeapBlock(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t length() const noexcept { return header().length(); }

    bool is_free() const noexcept { return header().is_free(); }

    uint64_t allocated_space() const noexcept { return address() + heap_block_size_; }

    struct Header : runtime::JvmObject
    {
        Header(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

        uint64_t length() const noexcept { return read_field<uint64_t>(length_offset_); }

        bool is_free() const noexcept { return !read_field<bool>(used_offset_); }
    };

  private:
    Header header() const noexcept { return address() + header_offset_; }

    DECLARE_STATIC_INIT

    static inline uint64_t header_offset_;

    static inline uint64_t heap_block_size_;

    static inline uint64_t length_offset_;
    static inline uint64_t used_offset_;
};
} // namespace hotspot::memory