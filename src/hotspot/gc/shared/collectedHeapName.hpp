#pragma once

#include <string_view>

namespace hotspot::gc::shared
{
enum class CollectedHeapName : uint8_t
{
    Serial,
    Parallel,
    CMS,
    G1,
    Epsilon,
    Z,
    Shenandoah
};

constexpr std::string_view to_string(CollectedHeapName name) noexcept
{
    switch (name)
    {
    case CollectedHeapName::Serial:
        return "Serial";
    case CollectedHeapName::Parallel:
        return "Parallel";
    case CollectedHeapName::CMS:
        return "CMS";
    case CollectedHeapName::G1:
        return "G1";
    case CollectedHeapName::Epsilon:
        return "Epsilon";
    case CollectedHeapName::Z:
        return "Z";
    case CollectedHeapName::Shenandoah:
        return "Shenandoah";
    }
    return "Unknown";
}
} // namespace hotspot::gc::shared