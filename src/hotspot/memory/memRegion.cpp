#include "memRegion.hpp"

namespace hotspot::memory
{
MemRegion::MemRegion(uint64_t addr) noexcept
    : MemRegion(runtime::Jvm::read<uint64_t>(addr + start_offset_),
                runtime::Jvm::read<uint64_t>(addr + word_size_offset_))
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
    return {std::min(start_, mr.start_), std::max(end(), mr.end()) - std::min(start_, mr.start_)};
}

void MemRegion::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("MemRegion");

    start_offset_ = *type->field_offset("_start");
    word_size_offset_ = *type->field_offset("_word_size");
}
} // namespace hotspot::memory