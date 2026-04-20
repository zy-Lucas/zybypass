#include "pcDesc.hpp"

namespace hotspot::code
{
PcDesc::PcDesc(uint64_t addr) : runtime::JvmObject(addr) {}

void PcDesc::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("PcDesc");

    pc_offset_offset = *type->get_field_offset("_pc_offset");
    scope_decode_offset_offset = *type->get_field_offset("_scope_decode_offset");
    obj_decode_offset_offset = *type->get_field_offset("_obj_decode_offset");
    flags_offset = *type->get_field_offset("_flags");

    reexecute_mask = *runtime::Jvm::lookup_int_constant("PcDesc::PCDESC_reexecute");
    is_method_handle_invoke_mask = *runtime::Jvm::lookup_int_constant("PcDesc::PCDESC_is_method_handle_invoke");
    return_oop_mask = *runtime::Jvm::lookup_int_constant("PcDesc::PCDESC_return_oop");

    pc_desc_size = type->get_size();
}
} // namespace hotspot::code