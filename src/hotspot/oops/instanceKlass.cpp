#include "instanceKlass.hpp"
#include "field.hpp"
#include "klassVtable.hpp"
#include "oops/method.hpp"
#include "vmSymbols.hpp"

namespace hotspot::oops
{
constexpr std::string_view ClassState::to_string() const noexcept
{
    switch (value_)
    {
    case Value::allocated:
        return "allocated";
    case Value::loaded:
        return "loaded";
    case Value::linked:
        return "linked";
    case Value::being_initialized:
        return "beingInitialized";
    case Value::fully_initialized:
        return "fullyInitialized";
    case Value::initialization_error:
        return "initializationError";
    }
    return "unknown";
}

uint64_t InstanceKlass::size() const noexcept
{
    int32_t word_length = runtime::Jvm::bytes_per_word();
    uint64_t size = header_size() + (vtable_length() + itable_length() + nonstatic_oop_map_size()) * word_length;
    if (is_interface())
        size += word_length;
    return align_size(size);
}

uint16_t InstanceKlass::field_access_flags(uint32_t index) const noexcept
{
    return fields().at(index * field_slots_ + access_flags_offset_);
}

Symbol InstanceKlass::field_name(uint32_t index) const noexcept
{
    uint16_t name_index = fields().at(index * field_slots_ + name_index_offset_);
    if (index < java_fields_count())
        return constants().symbol_at(name_index);
    return vmSymbols::symbol_at(name_index);
}

Symbol InstanceKlass::field_signature(uint32_t index) const noexcept
{
    uint16_t signature_index = fields().at(index * field_slots_ + signature_index_offset_);
    if (index < java_fields_count())
        return constants().symbol_at(signature_index);
    return vmSymbols::symbol_at(signature_index);
}

uint16_t InstanceKlass::field_generic_signature_index(uint32_t index) const noexcept
{
    uint32_t slot = all_fields_count() * field_slots_;
    for (uint32_t i = 0; i < index; ++i)
        slot += runtime::AccessFlags{field_access_flags(i)}.field_has_generic_signature();
    if (runtime::AccessFlags{field_access_flags(index)}.field_has_generic_signature())
        return fields().at(slot);
    return 0;
}

Symbol InstanceKlass::field_generic_signature(uint32_t index) const noexcept
{
    if (uint16_t generic_signature_index = field_generic_signature_index(index); generic_signature_index)
        return constants().symbol_at(generic_signature_index);
    return 0;
}

uint16_t InstanceKlass::field_initial_value_index(uint32_t index) const noexcept
{
    return fields().at(index * field_slots_ + initval_index_offset_);
}

uint32_t InstanceKlass::field_offset(uint32_t index) const noexcept
{
    utilities::U2Array arr{fields()};
    uint16_t lo = arr.at(index * field_slots_ + low_offset_);
    uint16_t hi = arr.at(index * field_slots_ + high_offset_);
    return runtime::Jvm::build_int_from_shorts(lo, hi) >> field_info_tag_size_;
}

uint16_t InstanceKlass::all_fields_count() const noexcept
{
    uint16_t all_fields_cunt = 0;
    for (uint16_t len = fields().length(); all_fields_cunt * field_slots_ < len; ++all_fields_cunt)
        if (runtime::AccessFlags{field_access_flags(all_fields_cunt)}.field_has_generic_signature())
            --len;
    return all_fields_cunt;
}

uint16_t InstanceKlass::next_method_idnum() noexcept
{
    if (method_idnum() == 0xFFFE)
        return 0xFFFF;
    write_field<uint16_t>(idnum_allocated_count_offset_, method_idnum() + 1);
    return method_idnum();
}

uint64_t *InstanceKlass::methods_jmethod_ids() const noexcept
{
    return atomic_load_field<uint64_t *>(methods_jmethod_ids_offset_);
}

uint64_t InstanceKlass::jmethod_id_or_null(Method method) const noexcept
{
    uint16_t idnum = method.method_idnum();
    uint64_t *jmeths = methods_jmethod_ids();
    uint64_t length;
    uint64_t id = 0;
    if (jmeths && (length = jmeths[0]) > idnum)
        id = jmeths[idnum + 1];
    return id;
}

void InstanceKlass::adjust_default_method(Method old_method, Method new_method) noexcept
{
    if (default_methods())
        for (uint32_t index = 0; index < default_methods().length(); ++index)
            if (default_methods().at(index) == old_method)
                default_methods().set_at(index, new_method.address());
}

utilities::KlassArray InstanceKlass::local_interfaces() const noexcept
{
    return read_field<uint64_t>(local_interfaces_offset_);
}

utilities::KlassArray InstanceKlass::transitive_interfaces() const noexcept
{
    return read_field<uint64_t>(transitive_interfaces_offset_);
}

klassItable InstanceKlass::itable() const noexcept { return klassItable(*this); }

std::string_view InstanceKlass::source_debug_extension_view() const noexcept
{
    return read_string_field(source_debug_extension_offset_);
}

Method InstanceKlass::find_method(std::string_view name, std::string_view sig) const noexcept
{

    int32_t hit = find_method_index(name, sig);
    return hit >= 0 ? methods().at(hit) : 0;
}

int32_t InstanceKlass::find_method_index(std::string_view name, std::string_view sig) const noexcept
{
    return linear_search(methods(), name, sig);
}

Field InstanceKlass::find_field(std::string_view name, std::string_view sig) const noexcept
{
    if (Field f{find_local_field(name, sig)}; f)
        return f;
    if (Field f{find_interface_field(name, sig)}; f)
        return f;
    if (InstanceKlass supr{super().address()}; supr)
        return supr.find_field(name, sig);
    return {0, 0};
}

Field InstanceKlass::find_local_field(std::string_view name, std::string_view sig) const noexcept
{
    uint16_t length = all_fields_count();
    for (uint32_t index = 0; index < length; ++index)
        if (field_name(index).equals(name) && field_signature(index).equals(sig))
            return Field{*this, index};
    return {0, 0};
}

Field InstanceKlass::find_interface_field(std::string_view name, std::string_view sig) const noexcept
{
    utilities::KlassArray interfaces{local_interfaces()};
    int32_t length = interfaces.length();
    for (uint32_t index = 0; index < length; ++index)
    {
        InstanceKlass intf{interfaces.at(index).address()};
        if (Field f{intf.find_local_field(name, sig)}; f)
            return f;
        if (Field f{intf.find_interface_field(name, sig)}; f)
            return f;
    }
    return {0, 0};
}

int32_t InstanceKlass::linear_search(utilities::MethodArray methods, std::string_view name,
                                     std::string_view signature) noexcept
{
    const int32_t len = methods.length();
    for (uint32_t index = 0; index < len; ++index)
        if (Method m{methods.at(index)}; m.name().equals(name) && m.signature().equals(signature))
            return index;
    return -1;
}

void InstanceKlass::initialize()
{
    utils::FieldResolver r{"InstanceKlass"};

    r.field_offset("_array_klasses", array_klasses_offset_);
    r.field_offset("_constants", constants_offset_);
    r.field_offset("_inner_classes", inner_classes_offset_);
    r.field_offset("_source_debug_extension", source_debug_extension_offset_);
    r.field_offset("_nonstatic_field_size", nonstatic_field_size_offset_);
    r.field_offset("_static_field_size", static_field_size_offset_);
    r.field_offset("_nonstatic_oop_map_size", nonstatic_oop_map_size_offset_);
    r.field_offset("_itable_len", itable_len_offset_);
    r.field_offset("_static_oop_field_count", static_oop_field_count_offset_);
    r.field_offset("_java_fields_count", java_fields_count_offset_);
    r.field_offset("_is_marked_dependent", is_marked_dependent_offset_);
    r.field_offset("_idnum_allocated_count", idnum_allocated_count_offset_);
    r.field_offset("_init_state", init_state_offset_);
    r.field_offset("_misc_flags", misc_flags_offset_);
    r.field_offset("_methods_jmethod_ids", methods_jmethod_ids_offset_);
    r.field_offset("_methods", methods_offset_);
    r.field_offset("_default_methods", default_methods_offset_);
    r.field_offset("_local_interfaces", local_interfaces_offset_);
    r.field_offset("_transitive_interfaces", transitive_interfaces_offset_);
    r.field_offset("_method_ordering", method_ordering_offset_);
    r.field_offset("_fields", fields_offset_);

    r.field_offset("_breakpoints", breakpoints_offset_);

    r.type_size(header_size_);

    utils::constants::int_const("FieldInfo::access_flags_offset", access_flags_offset_);
    utils::constants::int_const("FieldInfo::name_index_offset", name_index_offset_);
    utils::constants::int_const("FieldInfo::signature_index_offset", signature_index_offset_);
    utils::constants::int_const("FieldInfo::initval_index_offset", initval_index_offset_);
    utils::constants::int_const("FieldInfo::low_packed_offset", low_offset_);
    utils::constants::int_const("FieldInfo::high_packed_offset", high_offset_);
    utils::constants::int_const("FieldInfo::field_slots", field_slots_);
    utils::constants::int_const("FIELDINFO_TAG_SIZE", field_info_tag_size_);
    utils::constants::int_const("FIELDINFO_TAG_OFFSET", field_info_tag_offset_);

    utils::constants::int_const("InstanceKlass::allocated", class_state_allocated_);
    utils::constants::int_const("InstanceKlass::loaded", class_state_loaded_);
    utils::constants::int_const("InstanceKlass::linked", class_state_linked_);
    utils::constants::int_const("InstanceKlass::being_initialized", class_state_being_initialized_);
    utils::constants::int_const("InstanceKlass::fully_initialized", class_state_fully_initialized_);
    utils::constants::int_const("InstanceKlass::initialization_error", class_state_initialization_error_);

    utils::constants::int_const("InstanceKlass::_misc_rewritten", misc_rewritten_);
    utils::constants::int_const("InstanceKlass::_misc_has_nonstatic_fields", misc_has_nonstatic_fields_);
    utils::constants::int_const("InstanceKlass::_misc_should_verify_class", misc_should_verify_class_);
    utils::constants::int_const("InstanceKlass::_misc_is_contended", misc_is_contended_);
    utils::constants::int_const("InstanceKlass::_misc_has_nonstatic_concrete_methods",
                                misc_has_nonstatic_concrete_methods_);
    utils::constants::int_const("InstanceKlass::_misc_declares_nonstatic_concrete_methods",
                                misc_declares_nonstatic_concrete_methods_);
    utils::constants::int_const("InstanceKlass::_misc_has_been_redefined", misc_has_beed_redefined_);
    utils::constants::int_const("InstanceKlass::_misc_is_scratch_class", misc_is_scratch_class_);
    utils::constants::int_const("InstanceKlass::_misc_is_shared_boot_class", misc_is_shared_boot_class_);
    utils::constants::int_const("InstanceKlass::_misc_is_shared_platform_class", misc_is_shared_platform_class_);
    utils::constants::int_const("InstanceKlass::_misc_is_shared_app_class", misc_is_shared_app_class_);

    dep_context_offset_ = methods_jmethod_ids_offset_ + sizeof(void *);
}
} // namespace hotspot::oops