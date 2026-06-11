#include "reservedSpace.hpp"

namespace hotspot::memory
{
ReservedSpace::ReservedSpace(int8_t *base, uint64_t size, uint64_t alignment, uint64_t page_size, bool special,
                             bool executable) noexcept
    : fd_for_heap_(-1)
{
    initialize_members(base, size, alignment, page_size, special, executable);
}

void ReservedSpace::initialize_members(int8_t *base, uint64_t size, uint64_t alignment, uint64_t page_size,
                                       bool special, bool executable) noexcept
{
    base = base;
    size = size;
    alignment = alignment;
    page_size = page_size;
    special = special;
    executable = executable;
    noaccess_prefix_ = 0;
}

} // namespace hotspot::memory