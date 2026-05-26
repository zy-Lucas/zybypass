#pragma once

#include "symbol.hpp"

namespace hotspot::oops
{
class FieldType
{
  public:
    FieldType(Symbol signature) noexcept : signature_(signature), first_(signature.byte_at(0)) {}

    bool is_oop() const noexcept { return is_object() || is_array(); }
    bool is_byte() const noexcept { return first_ == 'B'; }
    bool is_char() const noexcept { return first_ == 'C'; }
    bool is_double() const noexcept { return first_ == 'D'; }
    bool is_float() const noexcept { return first_ == 'F'; }
    bool is_int() const noexcept { return first_ == 'I'; }
    bool is_long() const noexcept { return first_ == 'J'; }
    bool is_short() const noexcept { return first_ == 'S'; }
    bool is_boolean() const noexcept { return first_ == 'Z'; }
    bool is_object() const noexcept { return first_ == 'L'; }
    bool is_array() const noexcept { return first_ == '['; }

    Symbol signature() const noexcept { return signature_; }
    uint8_t tag() const noexcept { return first_; }

    struct ArrayInfo
    {
        int32_t dimension_;
        int32_t element_basic_type_;
    };
    ArrayInfo array_info() const noexcept;

  private:
    static void skip_optional_size(Symbol sig, uint32_t &index) noexcept;

    Symbol signature_;
    uint8_t first_;
};
} // namespace hotspot::oops