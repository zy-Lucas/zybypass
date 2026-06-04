#pragma once

#include "interpreter/interpreterCodelet.hpp"
#include "runtime/jvmObject.hpp"

namespace hotspot::code
{
struct EntryPoint
{
    enum Kind : uint8_t
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

    Kind kind_;

    constexpr EntryPoint(Kind kind) noexcept : kind_(kind) {}
    constexpr EntryPoint(uint8_t kind) noexcept : kind_(Kind(kind)) {}

    constexpr uint8_t raw() const noexcept { return kind_; }
    constexpr std::string_view to_string() const noexcept;

    constexpr bool operator==(const EntryPoint &) const noexcept = default;
    constexpr auto operator<=>(const EntryPoint &) const noexcept = default;
};

class StubQueue : public runtime::JvmObject
{
  public:
    StubQueue(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    bool contains(uint64_t pc) const noexcept;

    interpreter::InterpreterCodelet find_method_entry_point(EntryPoint entry_point) const noexcept;

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