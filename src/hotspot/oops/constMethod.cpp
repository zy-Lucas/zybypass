#include "constMethod.hpp"
#include <cstdint>

namespace hotspot::oops
{
ConstMethod::ConstMethod(uint64_t addr) : MetaData(addr) { STATIC_INIT_GUARD; }

uint64_t ConstMethod::offset_of_last_u2_element() const noexcept
{
    uint64_t offset =
        has_method_annotations() + has_parameter_annotations() + has_type_annotations() + has_default_annotations();
    int32_t word_size = *runtime::Jvm::get_oop_size();
    return (get_constMethod_size() * word_size) - (offset * word_size) - sizeof_short;
}

void ConstMethod::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstMethod");

    constants_offset = *type->get_field_offset("_constants");
    constMethod_size_offset = *type->get_field_offset("_constMethod_size");
    flags_offset = *type->get_field_offset("_flags");
    code_size_offset = *type->get_field_offset("_code_size");
    name_index_offset = *type->get_field_offset("_name_index");
    signature_index_offset = *type->get_field_offset("_signature_index");
    idnum_offset = *type->get_field_offset("_method_idnum");
    max_stack_offset = *type->get_field_offset("_max_stack");
    max_locals_offset = *type->get_field_offset("_max_locals");
    size_of_parameters_offset = *type->get_field_offset("_size_of_parameters");

    bytecode_offset = type->get_size();
    method_parameters_element_size = runtime::Jvm::lookup_type("MethodParametersElement")->get_size();
    checked_exception_element_size = runtime::Jvm::lookup_type("CheckedExceptionElement")->get_size();
    local_variable_table_element_size = runtime::Jvm::lookup_type("LocalVariableTableElement")->get_size();
    exception_table_element_size = runtime::Jvm::lookup_type("ExceptionTableElement")->get_size();

    HAS_LINENUMBER_TABLE = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_linenumber_table");
    HAS_CHECKED_EXCEPTIONS = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_checked_exceptions");
    HAS_LOCALVARIABLE_TABLE = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_localvariable_table");
    HAS_EXCEPTION_TABLE = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_exception_table");
    HAS_GENERIC_SIGNATURE = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_generic_signature");
    HAS_METHOD_PARAMETERS = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_method_parameters");
    HAS_METHOD_ANNOTATIONS = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_method_annotations");
    HAS_PARAMETER_ANNOTATIONS = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_parameter_annotations");
    HAS_DEFAULT_ANNOTATIONS = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_default_annotations");
    HAS_TYPE_ANNOTATIONS = *runtime::Jvm::lookup_int_constant("ConstMethod::_has_type_annotations");
}
} // namespace hotspot::oops