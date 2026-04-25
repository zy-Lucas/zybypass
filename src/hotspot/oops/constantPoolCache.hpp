#pragma once

#include "metaData.hpp"
#include <cstdint>

namespace hotspot::oops
{
class ConstantPool;

class ConstantPoolCache : public MetaData
{
  public:
    ConstantPoolCache(uint64_t addr);

    ConstantPool get_constants() const noexcept;

    uint64_t get_size() const noexcept { return align_size(base_offset + get_length() * element_size); }

    int32_t get_length() const noexcept { return read_field<int32_t>(length_offset); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t length_offset;
    static inline uint64_t constants_offset;
    static inline uint64_t resolved_references_offset;
    static inline uint64_t reference_map_offset;

    static inline uint64_t base_offset;

    static inline uint64_t element_size;
    static inline uint64_t int_size;
};
} // namespace hotspot::oops