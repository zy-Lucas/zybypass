#include "debugInfoReadStream.hpp"

namespace hotspot::code
{
DebugInfoReadStream::DebugInfoReadStream(nmethod code, uint32_t offset)
    : CompressedReadStream((const uint8_t *)code.scopes_data_begin(), offset), code(code)
{
}
}