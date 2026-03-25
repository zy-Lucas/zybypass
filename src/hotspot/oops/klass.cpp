#include "klass.hpp"

namespace hotspot::oops
{
Klass::Klass(uint64_t addr) : MetaData(addr) { std::call_once(init_flag_, initialize); }

bool Klass::is_subclass_of(const Klass &k) const noexcept
{
    for (Klass t = *this; t.address(); t = t.get_super())
        if (t == k)
            return true;
    return false;
}

void Klass::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Klass");

    java_mirror_offset = *type->get_field_offset("_java_mirror");
    super_offset = *type->get_field_offset("_super");
    layout_helper_offset = *type->get_field_offset("_layout_helper");
    name_offset = *type->get_field_offset("_name");
    access_flags_offset = *type->get_field_offset("_access_flags");
    subklass_offset = *type->get_field_offset("_subklass");
    next_sibling_offset = *type->get_field_offset("_next_sibling");
    next_link_offset = *type->get_field_offset("_next_link");
    vtable_len_offset = *type->get_field_offset("_vtable_len");
    class_loader_data_offset = *type->get_field_offset("_class_loader_data");

    trace_id_offset = type->get_field_offset("_trace_id");

    LH_INSTANCE_SLOW_PATH_BIT = *runtime::Jvm::lookup_int_constant("Klass::_lh_instance_slow_path_bit");
    LH_LOG2_ELEMENT_SIZE_SHIFT = *runtime::Jvm::lookup_int_constant("Klass::_lh_log2_element_size_shift");
    LH_LOG2_ELEMENT_SIZE_MASK = *runtime::Jvm::lookup_int_constant("Klass::_lh_log2_element_size_mask");
    LH_ELEMENT_TYPE_SHIFT = *runtime::Jvm::lookup_int_constant("Klass::_lh_element_type_shift");
    LH_HEADER_SIZE_SHIFT = *runtime::Jvm::lookup_int_constant("Klass::_lh_header_size_shift");
    LH_HEADER_SIZE_MASK = *runtime::Jvm::lookup_int_constant("Klass::_lh_header_size_mask");
    LH_ARRAY_TAG_SHIFT = *runtime::Jvm::lookup_int_constant("Klass::_lh_array_tag_shift");
    LH_ARRAY_TAG_TYPE_VALUE = *runtime::Jvm::lookup_int_constant("Klass::_lh_array_tag_type_value");
    LH_ARRAY_TAG_OBJ_VALUE = *runtime::Jvm::lookup_int_constant("Klass::_lh_array_tag_obj_value");
}
} // namespace hotspot::oops