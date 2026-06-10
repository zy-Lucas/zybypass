#include "klass.hpp"
#include "klassVtable.hpp"
#include "oops/instanceKlass.hpp"
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

uint64_t Klass::start_of_vtable() const noexcept { return address() + InstanceKlass::header_size(); }

klassVtable Klass::vtable() const noexcept { return klassVtable(*this, start_of_vtable(), vtable_length()); }

void Klass::initialize()
{
    utils::FieldResolver r{"Klass"};

    r.field_offset("_java_mirror", java_mirror_offset_);
    r.field_offset("_super", super_offset_);
    r.field_offset("_layout_helper", layout_helper_offset_);
    r.field_offset("_name", name_offset_);
    r.field_offset("_access_flags", access_flags_offset_);
    r.field_offset("_subklass", subklass_offset_);
    r.field_offset("_next_sibling", next_sibling_offset_);
    r.field_offset("_next_link", next_link_offset_);
    r.field_offset("_class_loader_data", class_loader_data_offset_);
    r.field_offset("_vtable_len", vtable_len_offset_);
    r.field_offset("_prototype_header", prototype_header_offset_);

    r.field_offset("_trace_id", trace_id_offset_);

    utils::constants::int_const("Klass::_lh_neutral_value", lh_neutral_value_);
    utils::constants::int_const("Klass::_lh_instance_slow_path_bit", lh_instance_slow_path_bit_);
    utils::constants::int_const("Klass::_lh_log2_element_size_shift", lh_log2_element_size_shift_);
    utils::constants::int_const("Klass::_lh_log2_element_size_mask", lh_log2_element_size_mask_);
    utils::constants::int_const("Klass::_lh_element_type_shift", lh_element_type_shift_);
    utils::constants::int_const("Klass::_lh_header_size_shift", lh_header_size_shift_);
    utils::constants::int_const("Klass::_lh_header_size_mask", lh_header_size_mask_);
    utils::constants::int_const("Klass::_lh_array_tag_shift", lh_array_tag_shift_);
    utils::constants::int_const("Klass::_lh_array_tag_type_value", lh_array_tag_type_value_);
    utils::constants::int_const("Klass::_lh_array_tag_obj_value", lh_array_tag_obj_value_);
}
} // namespace hotspot::oops