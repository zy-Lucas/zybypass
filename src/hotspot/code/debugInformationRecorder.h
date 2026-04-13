#pragma once

#include <cstdint>

namespace hotspot::code
{
enum DebugInformationRecorder : uint32_t
{
    SERIALIZED_NULL,
    SYNCHRONIZATION_ENTRY_BCI
};
} // namespace hotspot::code