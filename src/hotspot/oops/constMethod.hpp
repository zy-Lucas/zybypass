#pragma once

#include "constantPool.hpp"

namespace hotspot::oops
{
class ConstMethod : public MetaData
{
  public:
    ConstMethod(uint64_t addr) noexcept : MetaData(addr) {}

    Method method() const noexcept;
    ConstantPool constants() const noexcept { return read_field<uint64_t>(constants_offset_); }
    int32_t constMethod_size() const noexcept { return read_field<int32_t>(constMethod_size_offset_); }
    uint16_t flags() const noexcept { return read_field<uint16_t>(flags_offset_); }
    uint16_t code_size() const noexcept { return read_field<uint16_t>(code_size_offset_); }
    uint16_t name_index() const noexcept { return read_field<uint16_t>(name_index_offset_); }
    uint16_t signature_index() const noexcept { return read_field<uint16_t>(signature_index_offset_); }
    uint16_t idnum() const noexcept { return read_field<uint16_t>(idnum_offset_); }
    uint16_t max_stack() const noexcept { return read_field<uint16_t>(max_stack_offset_); }
    uint16_t max_locals() const noexcept { return read_field<uint16_t>(max_locals_offset_); }
    uint16_t size_of_parameters() const noexcept { return read_field<uint16_t>(size_of_parameters_offset_); }

    uint8_t u1_at(uint32_t bci) const noexcept { return read_field<uint8_t>(bytecode_offset_ + bci); }
    uint8_t opcode_at(uint32_t bci) const noexcept { return u1_at(bci); }

    uint16_t java_u2_at(uint32_t bci) const noexcept { return (u1_at(bci) << 8) | u1_at(bci + 1); }
    uint16_t native_u2_at(uint32_t bci) const noexcept { return read_field<uint16_t>(bytecode_offset_ + bci); }

    uint32_t java_u4_at(uint32_t bci) const noexcept { return java_u2_at(bci) << 16 | java_u2_at(bci + 1); }
    uint32_t native_u4_at(uint32_t bci) const noexcept { return read_field<uint32_t>(bytecode_offset_ + bci); }

    bool has_line_number_table() const noexcept { return flags() & has_line_number_table_; }

    bool has_local_variable_table() const noexcept { return flags() & has_local_variable_table_; }

    bool has_exception_table() const noexcept { return flags() & has_exception_table_; }

    bool has_checked_exceptions() const noexcept { return flags() & has_checked_exceptions_; }

  private:
    bool has_method_parameters() const noexcept { return flags() & has_method_parameters_; }
    bool has_generic_signature() const noexcept { return flags() & has_generic_signature_; }
    bool has_method_annotations() const noexcept { return flags() & has_method_annotations_; }
    bool has_parameter_annotations() const noexcept { return flags() & has_parameter_annotations_; }
    bool has_default_annotations() const noexcept { return flags() & has_default_annotations_; }
    bool has_type_annotations() const noexcept { return flags() & has_type_annotations_; }

    uint64_t offset_of_code_end() const noexcept { return bytecode_offset_ + code_size(); }

    uint64_t offset_of_last_u2_element() const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t constants_offset_;
    static inline uint64_t constMethod_size_offset_;
    static inline uint64_t flags_offset_;
    static inline uint64_t code_size_offset_;
    static inline uint64_t name_index_offset_;
    static inline uint64_t signature_index_offset_;
    static inline uint64_t idnum_offset_;
    static inline uint64_t max_stack_offset_;
    static inline uint64_t max_locals_offset_;
    static inline uint64_t size_of_parameters_offset_;

    static inline uint64_t bytecode_offset_;
    static inline uint64_t method_parameters_element_size_;
    static inline uint64_t checked_exception_element_size_;
    static inline uint64_t local_variable_table_element_size_;
    static inline uint64_t exception_table_element_size_;

    static inline int32_t has_line_number_table_;
    static inline int32_t has_checked_exceptions_;
    static inline int32_t has_local_variable_table_;
    static inline int32_t has_exception_table_;
    static inline int32_t has_generic_signature_;
    static inline int32_t has_method_parameters_;
    static inline int32_t has_method_annotations_;
    static inline int32_t has_parameter_annotations_;
    static inline int32_t has_default_annotations_;
    static inline int32_t has_type_annotations_;
};
} // namespace hotspot::oops