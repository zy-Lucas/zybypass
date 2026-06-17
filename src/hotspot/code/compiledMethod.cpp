#include "compiledMethod.hpp"

namespace hotspot::code
{
bool CompiledMethod::is_marked_for_deoptimization() const noexcept
{
    return read_field<int32_t>(mark_for_deoptimization_status_offset_);
}

void CompiledMethod::initialize()
{
    utils::FieldResolver r{"CompiledMethod"};
    utils::FieldResolver r_deop{"DeoptimizationBlob"};

    r_deop.field_offset("_unpack_offset", mark_for_deoptimization_status_offset_);
    r.field_offset("_method", method_offset_);
    r.field_offset("_scopes_data_begin", scopes_data_begin_offset_);
    r.field_offset("_deopt_handler_begin", deopt_handler_begin_offset_);
    r.field_offset("_deopt_mh_handler_begin", deopt_mh_handler_begin_offset_);
}
} // namespace hotspot::code