#include "scopeDesc.hpp"
#include "debugInfoReadStream.hpp"
#include <optional>

namespace hotspot::code
{
ScopeDesc::ScopeDesc(nmethod code, uint32_t decode_offset, uint32_t object_decode_offset, bool reexecute)
    : code(code), decode_offset(decode_offset), reexecute(reexecute)
{
    DebugInfoReadStream stream{code, decode_offset};

    sender_decode_offset = stream.read_int();
    method = stream.read_method();
    bci = stream.read_bic();

    locals_decode_offset = stream.read_int();
    expressions_decode_offset = stream.read_int();
    monitors_decode_offset = stream.read_int();
}

std::optional<ScopeDesc> ScopeDesc::sender() const noexcept
{
    return is_top() ? std::nullopt : std::make_optional<ScopeDesc>(code, sender_decode_offset, 0, false);
}
} // namespace hotspot::code