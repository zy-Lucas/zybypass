#include "codeCache.hpp"

namespace hotspot::code
{
bool CodeCache::contains(uint64_t p) noexcept
{
    for (uint32_t index = 0; index < heaps_.length(); ++index)
        if (heaps_.at(index).contains(p))
            return true;
    return false;
}

void CodeCache::initialize()
{
    utils::FieldResolver r{"CodeCache"};

    heaps_ = runtime::Jvm::read<uint64_t>(r.field_offset("_heaps"));
}
} // namespace hotspot::code