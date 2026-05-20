#include "constMethod.hpp"
#include "instanceKlass.hpp"
#include "method.hpp"

namespace hotspot::oops
{
Method ConstMethod::method() const noexcept
{
    return constants().pool_holder().methods().at(idnum());
}

uint64_t ConstMethod::offset_of_last_u2_element() const noexcept
{
    uint64_t offset =
        has_method_annotations() + has_parameter_annotations() + has_type_annotations() + has_default_annotations();
    int32_t word_size = runtime::Jvm::oop_size();
    return constMethod_size() * word_size - offset * word_size - sizeof(int16_t);
}

void ConstMethod::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstMethod");

    constants_offset_ = *type->field_offset("_constants");
    constMethod_size_offset_ = *type->field_offset("_constMethod_size");
    flags_offset_ = *type->field_offset("_flags");
    code_size_offset_ = *type->field_offset("_code_size");
    name_index_offset_ = *type->field_offset("_name_index");
    signature_index_offset_ = *type->field_offset("_signature_index");
    idnum_offset_ = *type->field_offset("_method_idnum");
    max_stack_offset_ = *type->field_offset("_max_stack");
    max_locals_offset_ = *type->field_offset("_max_locals");
    size_of_parameters_offset_ = *type->field_offset("_size_of_parameters");

    bytecode_offset_ = type->size();
    method_parameters_element_size_ = runtime::Jvm::lookup_type("MethodParametersElement")->size();
    checked_exception_element_size_ = runtime::Jvm::lookup_type("CheckedExceptionElement")->size();
    local_variable_table_element_size_ = runtime::Jvm::lookup_type("LocalVariableTableElement")->size();
    exception_table_element_size_ = runtime::Jvm::lookup_type("ExceptionTableElement")->size();

    has_line_number_table_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_linenumber_table");
    has_checked_exceptions_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_checked_exceptions");
    has_local_variable_table_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_localvariable_table");
    has_exception_table_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_exception_table");
    has_generic_signature_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_generic_signature");
    has_method_parameters_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_method_parameters");
    has_method_annotations_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_method_annotations");
    has_parameter_annotations_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_parameter_annotations");
    has_default_annotations_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_default_annotations");
    has_type_annotations_ = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_type_annotations");
}
} // namespace hotspot::oops