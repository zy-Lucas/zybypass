#pragma once

#include "../oops/oop.hpp"

namespace hotspot::classfile
{
class java_lang_String
{
  public:
    enum Coder
    {
        CODER_LATIN1 = 0,
        CODER_UTF16 = 1
    };

    static bool is_latin1(const oops::Instance &java_string) noexcept;
    static uint32_t length(const oops::Instance &java_string, const oops::TypeArray &value) noexcept;
    static uint32_t utf8_length(const oops::Instance &java_string, const oops::TypeArray &value) noexcept;

    static int8_t *as_utf8_string(const oops::Instance &java_string, const oops::TypeArray &value, int8_t *buf,
                                  uint32_t buflen) noexcept;

    static std::string to_std_string(const oops::Instance &java_string) noexcept;
};
} // namespace hotspot::classfile