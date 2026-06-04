#include "compiledMethod.hpp"

namespace hotspot::code
{
void CompiledMethod::initialize()
{
    utils::FieldResolver r{"CompiledMethod"};

    r.field_offset("_method", method_offset_);
    r.field_offset("_scopes_data_begin", scopes_data_begin_offset_);
    r.field_offset("_deopt_handler_begin", deopt_handler_begin_offset_);
    r.field_offset("_deopt_mh_handler_begin", deopt_mh_handler_begin_offset_);
}
} // namespace hotspot::code