#include "compressedStream.hpp"

namespace hotspot::code
{
CompressedStream::CompressedStream(const uint8_t *buffer, uint32_t position) noexcept
    : buffer(buffer), position(position)
{
}

uint32_t CompressedStream::reverse_int(uint32_t i) const noexcept
{
    i = (i & 0x55555555u) << 1 | (i >> 1) & 0x55555555u;
    i = (i & 0x33333333u) << 3 | (i >> 2) & 0x33333333u;
    i = (i & 0x0F0F0F0Fu) << 4 | (i >> 4) & 0x0F0F0F0Fu;
    i = (i << 24) | ((i & 0xFF00u) << 8) | ((i >> 8) & 0xFF00u) | (i >> 24);
    return i;
}

CompressedReadStream::CompressedReadStream(const uint8_t *buffer, uint32_t position) noexcept
    : CompressedStream(buffer, position)
{
}

uint32_t CompressedReadStream::read_int() noexcept
{
    uint8_t b0 = read();
    return b0 < L ? b0 : read_int_mb(b0);
}

double CompressedReadStream::read_double() noexcept
{
    uint64_t h = reverse_int(read_int());
    uint64_t l = reverse_int(read_int());
    return std::bit_cast<double>(h << 32 | l);
}

uint64_t CompressedReadStream::read_long() noexcept
{
    uint64_t h = read_signed_int();
    uint64_t l = read_signed_int();
    return h << 32 | l;
}

uint32_t CompressedReadStream::read_int_mb(uint8_t b0) noexcept
{
    uint32_t base_pos = position - 1;
    uint32_t sum = b0;
    int32_t shift = lg_H;
    for (int i = 0;;)
    {
        uint32_t b_i = read(base_pos + (++i));
        sum += b_i << shift;
        if (b_i < L || i == MAX_i)
        {
            set_position(base_pos + i + 1);
            return sum;
        }
        shift += lg_H;
    }
}
} // namespace hotspot::code