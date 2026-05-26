#include "klass.hpp"
#include "oop.hpp"
#include "vmOopHandle.hpp"

namespace hotspot::oops
{
Instance Klass::java_mirror() const { return VMOopHandle{address() + java_mirror_offset_}.resolve(); }

bool Klass::is_subclass_of(const Klass &k) const noexcept
{
    for (Klass t{*this}; t; t = t.super())
        if (t == k)
            return true;
    return false;
}

void Klass::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Klass");

    java_mirror_offset_ = *type->field_offset("_java_mirror");
    super_offset_ = *type->field_offset("_super");
    layout_helper_offset_ = *type->field_offset("_layout_helper");
    name_offset_ = *type->field_offset("_name");
    access_flags_offset_ = *type->field_offset("_access_flags");
    subklass_offset_ = *type->field_offset("_subklass");
    next_sibling_offset_ = *type->field_offset("_next_sibling");
    next_link_offset_ = *type->field_offset("_next_link");
    vtable_len_offset_ = *type->field_offset("_vtable_len");
    class_loader_data_offset_ = *type->field_offset("_class_loader_data");

    trace_id_offset_ = type->field_offset("_trace_id");

    lh_neutral_value_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_neutral_value");
    lh_instance_slow_path_bit_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_instance_slow_path_bit");
    lh_log2_element_size_shift_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_log2_element_size_shift");
    lh_log2_element_size_mask_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_log2_element_size_mask");
    lh_element_type_shift_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_element_type_shift");
    lh_header_size_shift_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_header_size_shift");
    lh_header_size_mask_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_header_size_mask");
    lh_array_tag_shift_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_array_tag_shift");
    lh_array_tag_type_value_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_array_tag_type_value");
    lh_array_tag_obj_value_ = *runtime::Jvm::lookup_int_constant("Klass::_lh_array_tag_obj_value");
}
} // namespace hotspot::oops