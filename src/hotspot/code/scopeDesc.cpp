#include "scopeDesc.hpp"
#include "debugInfoReadStream.hpp"

namespace hotspot::code
{
ScopeDesc::ScopeDesc(nmethod code, uint32_t decode_offset, uint32_t object_decode_offset, bool reexecute) noexcept
    : code_(code), decode_offset_(decode_offset), reexecute_(reexecute)
{
    DebugInfoReadStream stream{code, decode_offset};

    sender_decode_offset_ = stream.read_int();
    method_ = stream.read_method();
    bci_ = stream.read_bic();

    locals_decode_offset_ = stream.read_int();
    expressions_decode_offset_ = stream.read_int();
    monitors_decode_offset_ = stream.read_int();
}

ScopeDesc ScopeDesc::sender() const noexcept
{
    return is_top() ? 0 : ScopeDesc{code_, sender_decode_offset_, 0, false};
}
} // namespace hotspot::code