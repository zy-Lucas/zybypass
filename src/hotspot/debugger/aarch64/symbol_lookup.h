#pragma once

#include <stdint.h>

namespace hotspot::debugger::aarch64
{
#ifdef __cplusplus
extern "C"
{
#endif

    uint64_t lookup_by_name(const char *symbolName);

#ifdef __cplusplus
}
#endif
} // namespace hotspot::debugger::aarch64