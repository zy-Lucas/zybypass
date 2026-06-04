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
    utils::FieldResolver r{"MemRegion"};

    r.field_offset("_start", start_offset_);
    r.field_offset("_word_size", word_size_offset_);
}
} // namespace hotspot::memory