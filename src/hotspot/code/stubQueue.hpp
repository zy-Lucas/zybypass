#pragma once

#include "../interpreter/interpreterCodelet.hpp"
#include "../runtime/jvmObject.hpp"

namespace hotspot::code
{
enum class EntryPointKind : uint8_t
{
    zerolocals,
    zerolocals_synchronized,
    abstract,
    java_lang_math_sin,
    java_lang_math_cos,
    java_lang_math_tan,
    java_lang_math_abs,
    java_lang_math_sqrt,
    java_lang_math_log,
    java_lang_math_log10,
    java_lang_math_exp,
    java_lang_math_pow,
    java_lang_math_fmaF,
    java_lang_math_fmaD,
    java_lang_ref_reference_get,
    native,
    native_synchronized,
    java_util_zip_CRC32_update,
    java_util_zip_CRC32_updateBytes,
    java_util_zip_CRC32_updateByteBuffer,
    java_util_zip_CRC32C_updateBytes,
    java_util_zip_CRC32C_updateDirectByteBuffer,
    unknown
};

constexpr std::string_view to_string(EntryPointKind k) noexcept
{
    switch (k)
    {
    case EntryPointKind::zerolocals:
        return "method entry point (kind = zerolocals)";
    case EntryPointKind::zerolocals_synchronized:
        return "method entry point (kind = zerolocals_synchronized)";
    case EntryPointKind::abstract:
        return "method entry point (kind = abstract)";
    case EntryPointKind::java_lang_math_sin:
        return "method entry point (kind = java_lang_math_sin)";
    case EntryPointKind::java_lang_math_cos:
        return "method entry point (kind = java_lang_math_cos)";
    case EntryPointKind::java_lang_math_tan:
        return "method entry point (kind = java_lang_math_tan)";
    case EntryPointKind::java_lang_math_abs:
        return "method entry point (kind = java_lang_math_abs)";
    case EntryPointKind::java_lang_math_sqrt:
        return "method entry point (kind = java_lang_math_sqrt)";
    case EntryPointKind::java_lang_math_log:
        return "method entry point (kind = java_lang_math_log)";
    case EntryPointKind::java_lang_math_log10:
        return "method entry point (kind = java_lang_math_log10)";
    case EntryPointKind::java_lang_math_exp:
        return "method entry point (kind = java_lang_math_exp)";
    case EntryPointKind::java_lang_math_pow:
        return "method entry point (kind = java_lang_math_pow)";
    case EntryPointKind::java_lang_math_fmaF:
        return "method entry point (kind = java_lang_math_fmaF)";
    case EntryPointKind::java_lang_math_fmaD:
        return "method entry point (kind = java_lang_math_famD)";
    case EntryPointKind::java_lang_ref_reference_get:
        return "method entry point (kind = java_lang_ref_reference_get)";
    case EntryPointKind::native:
        return "method entry point (kind = native)";
    case EntryPointKind::native_synchronized:
        return "method entry point (kind = native_synchronized)";
    case EntryPointKind::java_util_zip_CRC32_update:
        return "method entry point (kind = java_util_zip_CRC32_update)";
    case EntryPointKind::java_util_zip_CRC32_updateBytes:
        return "method entry point (kind = java_util_zip_CRC32_updateBytes)";
    case EntryPointKind::java_util_zip_CRC32_updateByteBuffer:
        return "method entry point (kind = java_util_zip_CRC32_updateByteBuffer)";
    case EntryPointKind::java_util_zip_CRC32C_updateBytes:
        return "method entry point (kind = java_util_zip_CRC32C_updateBytes)";
    case EntryPointKind::java_util_zip_CRC32C_updateDirectByteBuffer:
        return "method entry point (kind = java_util_zip_CRC32C_updateDirectByteBuffer)";
    case EntryPointKind::unknown:
        return "unknown";
    }
}

class StubQueue : public runtime::JvmObject
{
  public:
    StubQueue(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    bool contains(uint64_t pc) const noexcept;

    interpreter::InterpreterCodelet find_method_entry_point(EntryPointKind kind) const noexcept;

    interpreter::InterpreterCodelet first() const noexcept { return number_of_stubs() ? stub_at(queue_begin()) : 0; }
    interpreter::InterpreterCodelet next(interpreter::InterpreterCodelet s) const noexcept;

    interpreter::InterpreterCodelet stub_containing(uint64_t pc) const noexcept;
    bool stub_contains(interpreter::InterpreterCodelet s, uint64_t pc) const noexcept;

    int32_t number_of_stubs() const noexcept { return read_field<int32_t>(number_of_stubs_offset_); }

  private:
    uint64_t stub_buffer() const noexcept { return read_field<uint64_t>(stub_buffer_offset_); }
    int32_t buffer_limit() const noexcept { return read_field<int32_t>(buffer_limit_offset_); }

    int32_t queue_begin() const noexcept { return read_field<int32_t>(queue_begin_offset_); }
    int32_t queue_end() const noexcept { return read_field<int32_t>(queue_end_offset_); }

    int32_t index_of(interpreter::InterpreterCodelet s) const noexcept { return s.address() - stub_buffer(); }
    interpreter::InterpreterCodelet stub_at(int32_t index) const noexcept { return stub_buffer() + index; };

    DECLARE_STATIC_INIT

    static inline uint64_t stub_buffer_offset_;
    static inline uint64_t buffer_limit_offset_;
    static inline uint64_t queue_begin_offset_;
    static inline uint64_t queue_end_offset_;
    static inline uint64_t number_of_stubs_offset_;
};
} // namespace hotspot::code