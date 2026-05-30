#pragma once

#include <cstdint>

namespace hotspot::utilities
{
class UNICODE
{
  public:
    static uint32_t utf8_size(int8_t c) noexcept { return c == 0x00 ? 2 : 1; }
    static uint32_t utf8_size(uint16_t c) noexcept { return (c == 0) + 1 + (c > 0x7F) + (c > 0x7FF); }

    template <typename T> static uint32_t utf8_length(const T *base, uint32_t length) noexcept;

    static int8_t *as_utf8(const int8_t *base, int length, int8_t *buf, int32_t buflen) noexcept;
    static int8_t *as_utf8(const uint16_t *base, int length, int8_t *buf, int32_t buflen) noexcept;

    static uint8_t *utf8_write(uint8_t *base, uint16_t ch) noexcept;
};

extern template uint32_t UNICODE::utf8_length(const int8_t *base, uint32_t length) noexcept;
extern template uint32_t UNICODE::utf8_length(const uint16_t *base, uint32_t length) noexcept;
} // namespace hotspot::utilities