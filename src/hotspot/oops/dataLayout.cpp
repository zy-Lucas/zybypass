#include "dataLayout.hpp"

namespace hotspot::oops
{
uint32_t DataLayout::compute_size_in_bytes(uint32_t cell_count) noexcept
{
    return header_size_in_bytes() + cell_count * sizeof(void *);
}
} // namespace hotspot::oops