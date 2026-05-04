#include "instanceKlass.hpp"
#include "method.hpp"

namespace hotspot::oops
{
constexpr std::string_view ClassState::to_string() const noexcept
{
    switch (value)
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

uint64_t InstanceKlass::get_size() const noexcept
{
    int32_t word_length = runtime::Jvm::get_bytes_per_word();
    uint64_t size =
        get_header_size() + (get_vtable_len() + get_vtable_len() + get_nonstatic_oop_map_size()) * word_length;
    if (is_interface())
        size += word_length;
    return align_size(size);
}

uint16_t InstanceKlass::get_field_access_flags(int32_t index) const noexcept
{
    return get_fields().at(index * FIELD_SLOTS + ACCESS_FLAGS_OFFSET);
}

uint16_t InstanceKlass::get_all_fields_count() const noexcept
{
    uint16_t allFieldsCount = 0;
    for (uint16_t len = get_fields().length(); allFieldsCount * FIELD_SLOTS < len; ++allFieldsCount)
        if (runtime::AccessFlags{get_field_access_flags(allFieldsCount)}.field_has_generic_signature())
            --len;
    return allFieldsCount;
}

utilities::KlassArray InstanceKlass::get_local_interfaces() const noexcept
{
    return read_field<uint64_t>(local_interfaces_offset);
}

utilities::KlassArray InstanceKlass::get_transitive_interfaces() const noexcept
{
    return read_field<uint64_t>(transitive_interfaces_offset);
}

std::string_view InstanceKlass::get_source_debug_extension_view() const noexcept
{
    return read_string_field(source_debug_extension_offset);
}

Method InstanceKlass::find_method(std::string_view name, std::string_view sig) const noexcept
{
    return find_method(get_methods(), name, sig);
}

Method InstanceKlass::find_method(utilities::MethodArray methods, std::string_view name,
                                  std::string_view signature) noexcept
{
    const int32_t len = methods.length();
    for (int32_t i = 0; i < len; ++i)
        if (Method m{methods.at(i)}; m.get_name().equals(name) && m.get_signature().equals(signature))
            return m;
    return 0;
}

void InstanceKlass::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("InstanceKlass");

    array_klasses_offset = *type->get_field_offset("_array_klasses");
    constants_offset = *type->get_field_offset("_constants");
    inner_classes_offset = *type->get_field_offset("_inner_classes");
    source_debug_extension_offset = *type->get_field_offset("_source_debug_extension");
    nonstatic_field_size_offset = *type->get_field_offset("_nonstatic_field_size");
    static_field_size_offset = *type->get_field_offset("_static_field_size");
    nonstatic_oop_map_size_offset = *type->get_field_offset("_nonstatic_oop_map_size");
    itable_len_offset = *type->get_field_offset("_itable_len");
    static_oop_field_count_offset = *type->get_field_offset("_static_oop_field_count");
    java_fields_count_offset = *type->get_field_offset("_java_fields_count");
    is_marked_dependent_offset = *type->get_field_offset("_is_marked_dependent");
    init_state_offset = *type->get_field_offset("_init_state");
    misc_flags_offset = *type->get_field_offset("_misc_flags");
    methods_offset = *type->get_field_offset("_methods");
    default_methods_offset = *type->get_field_offset("_default_methods");
    local_interfaces_offset = *type->get_field_offset("_local_interfaces");
    transitive_interfaces_offset = *type->get_field_offset("_transitive_interfaces");
    method_ordering_offset = *type->get_field_offset("_method_ordering");
    fields_offset = *type->get_field_offset("_fields");

    breakpoints_offset = type->get_field_offset("_breakpoints");

    header_size = type->get_size();

    ACCESS_FLAGS_OFFSET = *runtime::Jvm::lookup_int_constant("FieldInfo::access_flags_offset");
    NAME_INDEX_OFFSET = *runtime::Jvm::lookup_int_constant("FieldInfo::name_index_offset");
    SIGNATURE_INDEX_OFFSET = *runtime::Jvm::lookup_int_constant("FieldInfo::signature_index_offset");
    INITVAL_INDEX_OFFSET = *runtime::Jvm::lookup_int_constant("FieldInfo::initval_index_offset");
    LOW_OFFSET = *runtime::Jvm::lookup_int_constant("FieldInfo::low_packed_offset");
    HIGH_OFFSET = *runtime::Jvm::lookup_int_constant("FieldInfo::high_packed_offset");
    FIELD_SLOTS = *runtime::Jvm::lookup_int_constant("FieldInfo::field_slots");
    FIELDINFO_TAG_SIZE = *runtime::Jvm::lookup_int_constant("FIELDINFO_TAG_SIZE");
    FIELDINFO_TAG_OFFSET = *runtime::Jvm::lookup_int_constant("FIELDINFO_TAG_OFFSET");

    CLASS_STATE_ALLOCATED = *runtime::Jvm::lookup_int_constant("InstanceKlass::allocated");
    CLASS_STATE_LOADED = *runtime::Jvm::lookup_int_constant("InstanceKlass::loaded");
    CLASS_STATE_LINKED = *runtime::Jvm::lookup_int_constant("InstanceKlass::linked");
    CLASS_STATE_BEING_INITIALIZED = *runtime::Jvm::lookup_int_constant("InstanceKlass::being_initialized");
    CLASS_STATE_FULLY_INITIALIZED = *runtime::Jvm::lookup_int_constant("InstanceKlass::fully_initialized");
    CLASS_STATE_INITIALIZATION_ERROR = *runtime::Jvm::lookup_int_constant("InstanceKlass::initialization_error");

    MISC_REWRITTEN = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_rewritten");
    MISC_HAS_NONSTATIC_FIELDS = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_has_nonstatic_fields");
    MISC_SHOULD_VERIFY_CLASS = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_should_verify_class");
    MISC_IS_CONTENDED = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_is_contended");
    MISC_HAS_NONSTATIC_CONCRETE_METHODS =
        *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_has_nonstatic_concrete_methods");
    MISC_DECLARES_NONSTATIC_CONCRETE_METHODS =
        *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_declares_nonstatic_concrete_methods");
    MISC_HAS_BEEN_REDEFINED = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_has_been_redefined");
    MISC_IS_SCRATCH_CLASS = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_is_scratch_class");
    MISC_IS_SHARED_BOOT_CLASS = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_is_shared_boot_class");
    MISC_IS_SHARED_PLATFORM_CLASS = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_is_shared_platform_class");
    MISC_IS_SHARED_APP_CLASS = *runtime::Jvm::lookup_int_constant("InstanceKlass::_misc_is_shared_app_class");
}
} // namespace hotspot::oops