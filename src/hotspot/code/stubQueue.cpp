#include "stubQueue.hpp"
#include <string_view>

namespace hotspot::code
{
bool StubQueue::contains(uint64_t pc) const noexcept
{
    if (!pc)
        return false;
    int32_t offset = pc - stub_buffer();
    return offset >= 0 && offset < buffer_limit();
}

interpreter::InterpreterCodelet StubQueue::find_method_entry_point(EntryPointKind kind) const noexcept
{
    if (kind == EntryPointKind::unknown)
        return 0;
    std::string_view desc{to_string(kind)};
    for (hotspot::interpreter::InterpreterCodelet i{first()}; i; i = next(i))
        if (i.description_view() == desc)
            return i;
    return 0;
}

interpreter::InterpreterCodelet StubQueue::next(interpreter::InterpreterCodelet s) const noexcept
{
    int32_t i = index_of(s) + s.size();
    if (i == buffer_limit() && queue_end() < buffer_limit())
        i = 0;
    return i == queue_end() ? 0 : stub_at(i);
}

interpreter::InterpreterCodelet StubQueue::stub_containing(uint64_t pc) const noexcept
{
    if (contains(pc))
        for (interpreter::InterpreterCodelet s{first()}; s; s = next(s))
            if (stub_contains(s, pc))
                return s;
    return 0;
}

bool StubQueue::stub_contains(interpreter::InterpreterCodelet s, uint64_t pc) const noexcept
{
    return s.code_begin() <= pc && s.code_end() > pc;
}

void StubQueue::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("StubQueue");

    stub_buffer_offset_ = *type->field_offset("_stub_buffer");
    buffer_limit_offset_ = *type->field_offset("_buffer_limit");
    queue_begin_offset_ = *type->field_offset("_queue_begin");
    queue_end_offset_ = *type->field_offset("_queue_end");
    number_of_stubs_offset_ = *type->field_offset("_number_of_stubs");
}
} // namespace hotspot::code