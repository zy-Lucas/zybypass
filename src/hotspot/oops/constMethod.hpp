#pragma once

#include "constantPool.hpp"
#include "metaData.hpp"
#include <cstdint>

namespace hotspot::oops
{
class ConstMethod : public MetaData
{
  public:
    ConstMethod(uint64_t addr);

    ConstantPool get_constants() const noexcept { return read_field<uint64_t>(constants_offset); }
    int32_t get_constMethod_size() const noexcept { return read_field<int32_t>(constMethod_size_offset); }
    uint16_t get_flags() const noexcept { return read_field<uint16_t>(flags_offset); }
    uint16_t get_code_size() const noexcept { return read_field<uint16_t>(code_size_offset); }
    uint16_t get_name_index() const noexcept { return read_field<uint16_t>(name_index_offset); }
    uint16_t get_signature_index() const noexcept { return read_field<uint16_t>(signature_index_offset); }
    uint16_t get_idnum() const noexcept { return read_field<uint16_t>(idnum_offset); }
    uint16_t get_max_stack() const noexcept { return read_field<uint16_t>(max_stack_offset); }
    uint16_t get_max_locals() const noexcept { return read_field<uint16_t>(max_locals_offset); }
    uint16_t get_size_of_parameters() const noexcept { return read_field<uint16_t>(size_of_parameters_offset); }

    uint8_t get_u1_at(uint32_t bci) const noexcept { return read_field<uint8_t>(bytecode_offset + bci); }
    uint8_t get_opcode_at(uint32_t bci) const noexcept { return get_u1_at(bci); }

    uint16_t get_java_u2_at(uint32_t bci) const noexcept { return (get_u1_at(bci) << 8) | get_u1_at(bci + 1); }
    uint16_t get_native_u2_at(uint32_t bci) const noexcept { return read_field<uint16_t>(bytecode_offset + bci); }

    uint32_t get_java_u4_at(uint32_t bci) const noexcept { return get_java_u2_at(bci) << 16 | get_java_u2_at(bci + 1); }
    uint32_t get_native_u4_at(uint32_t bci) const noexcept { return read_field<uint32_t>(bytecode_offset + bci); }

    bool has_line_number_table() const noexcept { return get_flags() & HAS_LINENUMBER_TABLE; }

    bool has_local_variable_table() const noexcept { return get_flags() & HAS_LOCALVARIABLE_TABLE; }

    bool has_exception_table() const noexcept { return get_flags() & HAS_EXCEPTION_TABLE; }

    bool has_checked_exceptions() const noexcept { return get_flags() & HAS_CHECKED_EXCEPTIONS; }

  private:
    bool has_method_parameters() const noexcept { return get_flags() & HAS_METHOD_PARAMETERS; }
    bool has_generic_signature() const noexcept { return get_flags() & HAS_GENERIC_SIGNATURE; }
    bool has_method_annotations() const noexcept { return get_flags() & HAS_METHOD_ANNOTATIONS; }
    bool has_parameter_annotations() const noexcept { return get_flags() & HAS_PARAMETER_ANNOTATIONS; }
    bool has_default_annotations() const noexcept { return get_flags() & HAS_DEFAULT_ANNOTATIONS; }
    bool has_type_annotations() const noexcept { return get_flags() & HAS_TYPE_ANNOTATIONS; }

    uint64_t offset_of_code_end() const noexcept { return bytecode_offset + get_code_size(); }

    uint64_t offset_of_last_u2_element() const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t constants_offset;
    static inline uint64_t constMethod_size_offset;
    static inline uint64_t flags_offset;
    static inline uint64_t code_size_offset;
    static inline uint64_t name_index_offset;
    static inline uint64_t signature_index_offset;
    static inline uint64_t idnum_offset;
    static inline uint64_t max_stack_offset;
    static inline uint64_t max_locals_offset;
    static inline uint64_t size_of_parameters_offset;

    static inline uint64_t bytecode_offset;
    static inline uint64_t method_parameters_element_size;
    static inline uint64_t checked_exception_element_size;
    static inline uint64_t local_variable_table_element_size;
    static inline uint64_t exception_table_element_size;

    static inline int32_t HAS_LINENUMBER_TABLE;
    static inline int32_t HAS_CHECKED_EXCEPTIONS;
    static inline int32_t HAS_LOCALVARIABLE_TABLE;
    static inline int32_t HAS_EXCEPTION_TABLE;
    static inline int32_t HAS_GENERIC_SIGNATURE;
    static inline int32_t HAS_METHOD_PARAMETERS;
    static inline int32_t HAS_METHOD_ANNOTATIONS;
    static inline int32_t HAS_PARAMETER_ANNOTATIONS;
    static inline int32_t HAS_DEFAULT_ANNOTATIONS;
    static inline int32_t HAS_TYPE_ANNOTATIONS;

    static constexpr int32_t sizeof_short = 2;
};
} // namespace hotspot::oops