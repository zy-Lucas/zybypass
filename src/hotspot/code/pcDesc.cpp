#include "pcDesc.hpp"

namespace hotspot::code
{
void PcDesc::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("PcDesc");

    pc_offset_offset_ = *type->field_offset("_pc_offset");
    scope_decode_offset_offset_ = *type->field_offset("_scope_decode_offset");
    obj_decode_offset_offset_ = *type->field_offset("_obj_decode_offset");
    flags_offset_ = *type->field_offset("_flags");

    pc_desc_size_ = type->size();

    reexecute_mask_ = *runtime::Jvm::lookup_int_constant("PcDesc::PCDESC_reexecute");
    is_method_handle_invoke_mask_ = *runtime::Jvm::lookup_int_constant("PcDesc::PCDESC_is_method_handle_invoke");
    return_oop_mask_ = *runtime::Jvm::lookup_int_constant("PcDesc::PCDESC_return_oop");
}
} // namespace hotspot::code