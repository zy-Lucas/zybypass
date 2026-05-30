#pragma once

#include "arrayKlass.hpp"

namespace hotspot::oops
{
class TypeArrayKlass : public ArrayKlass
{
  public:
    TypeArrayKlass(uint64_t addr) noexcept : ArrayKlass(addr) {}

    int32_t max_length() const noexcept { return read_field<int32_t>(max_length_offset_); }

    std::string_view type_name() const noexcept;
    std::string_view element_eype_name() const noexcept;

    static constexpr int32_t T_BOOLEAN = 4;
    static constexpr int32_t T_CHAR = 5;
    static constexpr int32_t T_FLOAT = 6;
    static constexpr int32_t T_DOUBLE = 7;
    static constexpr int32_t T_BYTE = 8;
    static constexpr int32_t T_SHORT = 9;
    static constexpr int32_t T_INT = 10;
    static constexpr int32_t T_LONG = 11;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t max_length_offset_;
};
} // namespace hotspot::oops