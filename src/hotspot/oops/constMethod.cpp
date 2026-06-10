#include "constMethod.hpp"
#include "instanceKlass.hpp"
#include "method.hpp"

namespace hotspot::oops
{
Method ConstMethod::method() const noexcept { return constants().pool_holder().methods().at(method_idnum()); }

uint64_t ConstMethod::offset_of_last_u2_element() const noexcept
{
    uint64_t offset =
        has_method_annotations() + has_parameter_annotations() + has_type_annotations() + has_default_annotations();
    int32_t word_size = runtime::Jvm::oop_size();
    return constMethod_size() * word_size - offset * word_size - sizeof(int16_t);
}

void ConstMethod::initialize()
{
    utils::FieldResolver r{"ConstMethod"};

    r.field_offset("_constants", constants_offset_);
    r.field_offset("_constMethod_size", constMethod_size_offset_);
    r.field_offset("_flags", flags_offset_);
    r.field_offset("_code_size", code_size_offset_);
    r.field_offset("_name_index", name_index_offset_);
    r.field_offset("_signature_index", signature_index_offset_);
    r.field_offset("_method_idnum", method_idnum_offset_);
    r.field_offset("_max_stack", max_stack_offset_);
    r.field_offset("_max_locals", max_locals_offset_);
    r.field_offset("_size_of_parameters", size_of_parameters_offset_);

    r.type_size(bytecode_offset_);

    utils::FieldResolver{"MethodParametersElement"}.type_size(method_parameters_element_size_);
    utils::FieldResolver{"CheckedExceptionElement"}.type_size(checked_exception_element_size_);
    utils::FieldResolver{"LocalVariableTableElement"}.type_size(local_variable_table_element_size_);
    utils::FieldResolver{"ExceptionTableElement"}.type_size(exception_table_element_size_);

    utils::constants::int_const("ConstMethod::_has_linenumber_table", has_line_number_table_);
    utils::constants::int_const("ConstMethod::_has_checked_exceptions", has_checked_exceptions_);
    utils::constants::int_const("ConstMethod::_has_localvariable_table", has_local_variable_table_);
    utils::constants::int_const("ConstMethod::_has_exception_table", has_exception_table_);
    utils::constants::int_const("ConstMethod::_has_generic_signature", has_generic_signature_);
    utils::constants::int_const("ConstMethod::_has_method_parameters", has_method_parameters_);
    utils::constants::int_const("ConstMethod::_has_method_annotations", has_method_annotations_);
    utils::constants::int_const("ConstMethod::_has_parameter_annotations", has_parameter_annotations_);
    utils::constants::int_const("ConstMethod::_has_default_annotations", has_default_annotations_);
    utils::constants::int_const("ConstMethod::_has_type_annotations", has_type_annotations_);
}
} // namespace hotspot::oops