#include "stubQueue.hpp"
#include <search.h>
#include <string_view>

namespace hotspot::code
{
constexpr std::string_view EntryPoint::to_string() const noexcept
{
    switch (kind_)
    {
    case Kind::zerolocals:
        return "method entry point (kind = zerolocals)";
    case Kind::zerolocals_synchronized:
        return "method entry point (kind = zerolocals_synchronized)";
    case Kind::abstract:
        return "method entry point (kind = abstract)";
    case Kind::java_lang_math_sin:
        return "method entry point (kind = java_lang_math_sin)";
    case Kind::java_lang_math_cos:
        return "method entry point (kind = java_lang_math_cos)";
    case Kind::java_lang_math_tan:
        return "method entry point (kind = java_lang_math_tan)";
    case Kind::java_lang_math_abs:
        return "method entry point (kind = java_lang_math_abs)";
    case Kind::java_lang_math_sqrt:
        return "method entry point (kind = java_lang_math_sqrt)";
    case Kind::java_lang_math_log:
        return "method entry point (kind = java_lang_math_log)";
    case Kind::java_lang_math_log10:
        return "method entry point (kind = java_lang_math_log10)";
    case Kind::java_lang_math_exp:
        return "method entry point (kind = java_lang_math_exp)";
    case Kind::java_lang_math_pow:
        return "method entry point (kind = java_lang_math_pow)";
    case Kind::java_lang_math_fmaF:
        return "method entry point (kind = java_lang_math_fmaF)";
    case Kind::java_lang_math_fmaD:
        return "method entry point (kind = java_lang_math_famD)";
    case Kind::java_lang_ref_reference_get:
        return "method entry point (kind = java_lang_ref_reference_get)";
    case Kind::native:
        return "method entry point (kind = native)";
    case Kind::native_synchronized:
        return "method entry point (kind = native_synchronized)";
    case Kind::java_util_zip_CRC32_update:
        return "method entry point (kind = java_util_zip_CRC32_update)";
    case Kind::java_util_zip_CRC32_updateBytes:
        return "method entry point (kind = java_util_zip_CRC32_updateBytes)";
    case Kind::java_util_zip_CRC32_updateByteBuffer:
        return "method entry point (kind = java_util_zip_CRC32_updateByteBuffer)";
    case Kind::java_util_zip_CRC32C_updateBytes:
        return "method entry point (kind = java_util_zip_CRC32C_updateBytes)";
    case Kind::java_util_zip_CRC32C_updateDirectByteBuffer:
        return "method entry point (kind = java_util_zip_CRC32C_updateDirectByteBuffer)";
    case Kind::unknown:
        return "unknown";
    }
}

bool StubQueue::contains(uint64_t pc) const noexcept
{
    if (!pc)
        return false;
    int32_t offset = pc - stub_buffer();
    return offset >= 0 && offset < buffer_limit();
}

interpreter::InterpreterCodelet StubQueue::find_method_entry_point(EntryPoint entry_point) const noexcept
{
    if (entry_point == EntryPoint::unknown)
        return 0;
    std::string_view desc{entry_point.to_string()};
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
    utils::FieldResolver r{"StubQueue"};

    r.field_offset("_stub_buffer", stub_buffer_offset_);
    r.field_offset("_buffer_limit", buffer_limit_offset_);
    r.field_offset("_queue_begin", queue_begin_offset_);
    r.field_offset("_queue_end", queue_end_offset_);
    r.field_offset("_number_of_stubs", number_of_stubs_offset_);
}
} // namespace hotspot::code