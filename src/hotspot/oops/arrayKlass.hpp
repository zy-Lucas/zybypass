#pragma once

#include "klass.hpp"

namespace hotspot::oops
{
class ArrayKlass : public Klass
{
  public:
    int32_t get_dimension() const noexcept { return read_field<int32_t>(dimension_offset); }
    Klass get_higher_dimension() const noexcept { return read_field<uint64_t>(higher_dimension_offset); }
    Klass get_lower_dimension() const noexcept { return read_field<uint64_t>(lower_dimension_offset); }

    uint32_t get_array_header_in_bytes() const noexcept { return get_layout_helper() >> LH_HEADER_SIZE_SHIFT & 0xFF; }
    uint32_t get_log2_element_size() const noexcept { return get_layout_helper() >> LH_LOG2_ELEMENT_SIZE_SHIFT & 0xFF; }
    uint32_t get_element_type() const noexcept { return get_layout_helper() >> LH_ELEMENT_TYPE_SHIFT & 0xFF; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t dimension_offset;
    static inline uint64_t higher_dimension_offset;
    static inline uint64_t lower_dimension_offset;
};
} // namespace hotspot::oops