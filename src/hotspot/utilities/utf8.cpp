#include "utf8.hpp"
#include <limits>

namespace hotspot::utilities
{
template <typename T> uint32_t UNICODE::utf8_length(const T *base, uint32_t length) noexcept
{
    uint32_t result = 0;
    for (uint32_t index = 0; index < length; ++index)
    {
        T c = base[index];
        uint32_t sz = utf8_size(c);
        if (result + sz > std::numeric_limits<int>::max() - 1)
            break;
        result += sz;
    }
    return result;
}

int8_t *UNICODE::as_utf8(const int8_t *base, int length, int8_t *buf, int32_t buflen) noexcept
{
    uint8_t *p = (uint8_t *)buf;
    for (uint32_t index = 0; index < length; ++index)
    {
        int8_t c = base[index];
        uint32_t sz = utf8_size(c);
        buflen -= sz;
        if (buflen <= 0)
            break;
        if (sz == 1)
            *p++ = c;
        else
            p = utf8_write(p, (uint16_t)c & 0xff);
    }
    *p = '\0';
    return buf;
}

int8_t *UNICODE::as_utf8(const uint16_t *base, int length, int8_t *buf, int32_t buflen) noexcept
{
    uint8_t *p = (uint8_t *)buf;
    for (uint32_t index = 0; index < length; ++index)
    {
        uint16_t c = base[index];
        buflen -= utf8_size(c);
        if (buflen <= 0)
            break;
        p = utf8_write(p, c);
    }
    *p = '\0';
    return buf;
}

uint8_t *UNICODE::utf8_write(uint8_t *base, uint16_t ch) noexcept
{
    if (ch != 0 && ch <= 0x7f)
    {
        base[0] = (uint8_t)ch;
        return base + 1;
    }

    if (ch <= 0x7FF)
    {
        uint8_t high_five = ch >> 6;
        uint8_t low_six = ch & 0x3F;
        base[0] = high_five | 0xC0;
        base[1] = low_six | 0x80;
        return base + 2;
    }
    int8_t high_four = ch >> 12;
    int8_t mid_six = (ch >> 6) & 0x3F;
    int8_t low_six = ch & 0x3f;
    base[0] = high_four | 0xE0;
    base[1] = mid_six | 0x80;
    base[2] = low_six | 0x80;
    return base + 3;
}

template uint32_t UNICODE::utf8_length(const int8_t *base, uint32_t length) noexcept;
template uint32_t UNICODE::utf8_length(const uint16_t *base, uint32_t length) noexcept;
} // namespace hotspot::utilities