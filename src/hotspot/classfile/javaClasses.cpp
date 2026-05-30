#include "javaClasses.hpp"
#include "../oops/field.hpp"
#include "../utilities/utf8.hpp"

namespace hotspot::classfile
{
bool java_lang_String::is_latin1(const oops::Instance &java_string) noexcept
{
    static uint32_t coder_index{
        oops::InstanceKlass{java_string.klass().address()}.find_field("coder", "B").field_index()};
    oops::ByteField field{oops::InstanceKlass{java_string.klass().address()}, coder_index};
    return field.value(java_string) == CODER_LATIN1;
}

uint32_t java_lang_String::length(const oops::Instance &java_string, const oops::TypeArray &value) noexcept
{
    if (!value)
        return 0;
    uint32_t arr_length = value.length();
    if (!is_latin1(java_string))
        arr_length >>= 1;
    return arr_length;
}

uint32_t java_lang_String::utf8_length(const hotspot::oops::Instance &java_string,
                                       const hotspot::oops::TypeArray &value) noexcept
{
    uint32_t len = length(java_string, value);
    if (!len)
        return 0;
    if (is_latin1(java_string))
        return utilities::UNICODE::utf8_length(value.byte_at_addr(0), len);
    return utilities::UNICODE::utf8_length(value.char_at_addr(0), len);
}

int8_t *java_lang_String::as_utf8_string(const oops::Instance &java_string, const oops::TypeArray &value, int8_t *buf,
                                         uint32_t buflen) noexcept
{
    uint32_t len = length(java_string, value);
    if (!len)
        return 0;
    if (is_latin1(java_string))
        return utilities::UNICODE::as_utf8(value.byte_at_addr(0), len, buf, buflen);
    return utilities::UNICODE::as_utf8(value.char_at_addr(0), len, buf, buflen);
}

std::string java_lang_String::to_std_string(const oops::Instance &java_string) noexcept
{
    static uint32_t value_index{
        oops::InstanceKlass{java_string.klass().address()}.find_field("value", "[B").field_index()};
    oops::OopField field{oops::InstanceKlass{java_string.klass().address()}, value_index};
    if (oops::TypeArray value{field.value(java_string)}; value)
    {
        uint32_t len = utf8_length(java_string, value) + 1;
        std::vector<int8_t> buf(len);
        as_utf8_string(java_string, value, buf.data(), len);
        return std::string{std::string_view{(const char *)buf.data()}};
    }
    return {};
}
} // namespace hotspot::classfile