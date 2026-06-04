#include "codeCache.hpp"

namespace hotspot::code
{
bool CodeCache::contains(uint64_t p) noexcept
{
    for (uint32_t i = 0; i < heaps_.length(); ++i)
        if (heaps_.at(i).contains(p))
            return true;
    return false;
}

void CodeCache::initialize()
{
    utils::FieldResolver r{"CodeCache"};

    heaps_ = runtime::Jvm::read<uint64_t>(r.field_offset("_heaps"));
}
} // namespace hotspot::code