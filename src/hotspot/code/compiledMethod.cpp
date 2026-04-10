#include "compiledMethod.hpp"

namespace hotspot::code
{
CompiledMethod::CompiledMethod(uint64_t addr) : CodeBlob(addr) { STATIC_INIT_GUARD; }

void CompiledMethod::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CompiledMethod");

    method_offset = *type->get_field_offset("_method");
    scopes_data_begin_offset = *type->get_field_offset("_scopes_data_begin");
    deopt_handler_begin_offset = *type->get_field_offset("_deopt_handler_begin");
    deopt_mh_handler_begin_offset = *type->get_field_offset("_deopt_mh_handler_begin");
}
} // namespace hotspot::code