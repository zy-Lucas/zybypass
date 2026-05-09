#include "codeCache.hpp"

namespace hotspot::code
{
bool CodeCache::contains(uint64_t p) noexcept
{
    for (uint32_t i = 0; i < heaps.length(); ++i)
        if (heaps.at(i).contains(p))
            return true;
    return false;
}

void CodeCache::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CodeCache");

    heaps = runtime::Jvm::read<uint64_t>(*type->get_field_offset("_heaps"));
}
} // namespace hotspot::code