#pragma once

#include "klass.hpp"

namespace hotspot::oops
{
class ArrayKlass : public Klass
{
  public:
    ArrayKlass(uint64_t addr) noexcept : Klass(addr) {}
    
    int32_t dimension() const noexcept { return read_field<int32_t>(dimension_offset_); }
    Klass higher_dimension() const noexcept { return read_field<uint64_t>(higher_dimension_offset_); }
    Klass lower_dimension() const noexcept { return read_field<uint64_t>(lower_dimension_offset_); }

    uint32_t array_header_in_bytes() const noexcept { return layout_helper() >> lh_header_size_shift_ & 0xFF; }
    uint32_t log2_element_size() const noexcept { return layout_helper() >> lh_log2_element_size_shift_ & 0xFF; }
    uint32_t element_type() const noexcept { return layout_helper() >> lh_element_type_shift_ & 0xFF; }

    bool compute_subtype_of(Klass k) const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t dimension_offset_;
    static inline uint64_t higher_dimension_offset_;
    static inline uint64_t lower_dimension_offset_;
};
} // namespace hotspot::oops