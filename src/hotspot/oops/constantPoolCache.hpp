#pragma once

#include "metaData.hpp"

namespace hotspot::oops
{
class ConstantPool;

class ConstantPoolCache : public MetaData
{
  public:
    ConstantPoolCache(uint64_t addr) noexcept : MetaData(addr) {}

    ConstantPool constants() const noexcept;

    uint64_t size() const noexcept { return align_size(base_offset_ + length() * element_size_); }

    int32_t length() const noexcept { return read_field<int32_t>(length_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t length_offset_;
    static inline uint64_t constants_offset_;
    static inline uint64_t resolved_references_offset_;
    static inline uint64_t reference_map_offset_;

    static inline uint64_t base_offset_;

    static inline uint64_t element_size_;
};
} // namespace hotspot::oops