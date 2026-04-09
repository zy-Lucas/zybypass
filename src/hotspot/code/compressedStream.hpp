#pragma once

#include <bit>
#include <cstdint>

namespace hotspot::code
{
class CompressedStream
{
  public:
    static constexpr int32_t LogBitsPerByte = 3;
    static constexpr int32_t BitsPerByte = 1 << LogBitsPerByte;

    static constexpr int32_t lg_H = 6;
    static constexpr int32_t H = 1 << lg_H;
    static constexpr int32_t L = (1 << BitsPerByte) - H;
    static constexpr int32_t MAX_i = 4;

    CompressedStream(const uint8_t *buffer, uint32_t position) noexcept;

    const uint8_t *get_buffer() const noexcept { return buffer; }

    uint32_t get_position() const noexcept { return position; }
    void set_position(uint32_t new_position) noexcept { position = new_position; }

    uint32_t encode_sign(uint32_t value) const noexcept { return (value << 1) ^ (value >> 31); }
    uint32_t decode_sign(uint32_t value) const noexcept { return (value >> 1u) ^ -(value & 1); }

    uint32_t reverse_int(uint32_t i) const noexcept;

  protected:
    const uint8_t *buffer;
    uint32_t position;
};

class CompressedReadStream : public CompressedStream
{
  public:
    CompressedReadStream(const uint8_t *buffer, uint32_t position = 0) noexcept;

    bool read_bool() noexcept { return read(); }
    uint8_t read_byte() noexcept { return read(); }
    uint16_t read_char() noexcept { return read(); }
    int16_t read_short() noexcept { return read_signed_int(); }
    int32_t read_signed_int() noexcept { return decode_sign(read_int()); }
    uint32_t read_int() noexcept;
    float read_float() noexcept { return std::bit_cast<float>(reverse_int(read_int())); }
    double read_double() noexcept;
    uint64_t read_long() noexcept;

  private:
    uint32_t read_int_mb(uint8_t b0) noexcept;
    uint8_t read(uint32_t index) const noexcept { return buffer[index]; }
    uint8_t read() noexcept { return buffer[position++]; }
};
} // namespace hotspot::code