#include "memRegion.hpp"

namespace hotspot::memory
{
MemRegion::MemRegion(uint64_t addr)
    : MemRegion(runtime::Jvm::read<uint64_t>(addr + start_offset),
                runtime::Jvm::read<uint64_t>(addr + word_size_offset))
{
}

MemRegion MemRegion::intersection(const MemRegion &mr) const noexcept
{
    uint64_t s = std::max(start(), mr.start());
    uint64_t e = std::min(end(), mr.end());
    if (e < s)
        return MemRegion{};
    return {s, e - s};
}

MemRegion MemRegion::merge(const MemRegion &mr) const noexcept
{
    return MemRegion{std::min(_start, mr._start), std::max(end(), mr.end()) - std::min(_start, mr._start)};
}

void MemRegion::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("MemRegion");

    start_offset = *type->get_field_offset("_start");
    word_size_offset = *type->get_field_offset("_word_size");
}
} // namespace hotspot::memory