#include "pcDesc.hpp"

namespace hotspot::code
{
void PcDesc::initialize()
{
    utils::FieldResolver r{"PcDesc"};

    r.field_offset("_pc_offset", pc_offset_offset_);
    r.field_offset("_scope_decode_offset", scope_decode_offset_offset_);
    r.field_offset("_obj_decode_offset", obj_decode_offset_offset_);
    r.field_offset("_flags", flags_offset_);

    r.type_size(pc_desc_size_);

    utils::constants::int_const("PcDesc::PCDESC_reexecute", reexecute_mask_);
    utils::constants::int_const("PcDesc::PCDESC_is_method_handle_invoke", is_method_handle_invoke_mask_);
    utils::constants::int_const("PcDesc::PCDESC_return_oop", return_oop_mask_);
}
} // namespace hotspot::code