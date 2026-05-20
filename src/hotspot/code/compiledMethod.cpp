#include "compiledMethod.hpp"

namespace hotspot::code
{
void CompiledMethod::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CompiledMethod");

    method_offset_ = *type->field_offset("_method");
    scopes_data_begin_offset_ = *type->field_offset("_scopes_data_begin");
    deopt_handler_begin_offset_ = *type->field_offset("_deopt_handler_begin");
    deopt_mh_handler_begin_offset_ = *type->field_offset("_deopt_mh_handler_begin");
}
} // namespace hotspot::code