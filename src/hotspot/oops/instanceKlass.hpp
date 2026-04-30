#pragma once

#include "constantPool.hpp"
#include "klass.hpp"

namespace hotspot::oops
{
struct ClassState
{
    enum Value : uint8_t
    {
        allocated,
        loaded,
        linked,
        being_initialized,
        fully_initialized,
        initialization_error
    };

    Value value;

    constexpr ClassState(Value v) noexcept : value(v) {}
    constexpr ClassState(uint8_t v) noexcept : value(Value(v)) {}

    constexpr uint8_t raw() const noexcept { return value; }
    constexpr std::string_view to_string() const noexcept;

    constexpr bool is_loaded() const noexcept { return value >= Value::loaded; }
    constexpr bool is_linked() const noexcept { return value >= Value::linked; }
    constexpr bool is_initialized() const noexcept { return value == Value::fully_initialized; }
    constexpr bool is_not_initialized() const noexcept { return value < Value::being_initialized; }
    constexpr bool is_being_initialized() const noexcept { return value == Value::being_initialized; }
    constexpr bool is_in_error_state() const noexcept { return value == Value::initialization_error; }

    constexpr auto operator<=>(const ClassState &) const noexcept = default;
    constexpr bool operator==(const ClassState &) const noexcept = default;

    static constexpr Value ALLOCATED = Value::allocated;
    static constexpr Value LOADED = Value::loaded;
    static constexpr Value LINKED = Value::linked;
    static constexpr Value BEING_INITIALIZED = Value::being_initialized;
    static constexpr Value FULLY_INITIALIZED = Value::fully_initialized;
    static constexpr Value INITIALIZATION_ERROR = Value::initialization_error;
};

class InstanceKlass : public Klass
{
  public:
    InstanceKlass(uint64_t addr) : Klass(addr) {}

    ClassState get_init_state() const noexcept { return read_field<uint8_t>(init_state_offset); }

    bool is_loaded() const noexcept { return get_init_state().is_loaded(); }
    bool is_linked() const noexcept { return get_init_state().is_linked(); }
    bool is_initialized() const noexcept { return get_init_state().is_initialized(); }
    bool is_not_initialized() const noexcept { return get_init_state().is_not_initialized(); }
    bool is_being_initialized() const noexcept { return get_init_state().is_being_initialized(); }
    bool is_in_error_state() const noexcept { return get_init_state().is_in_error_state(); }

    uint64_t get_size() const noexcept;

    uint16_t get_field_access_flags(int32_t index) const noexcept;

    ConstantPool get_constants() const noexcept { return read_field<uint64_t>(constants_offset); }
    uint16_t major_version() const noexcept { return get_constants().get_major(); }
    uint16_t minor_version() const noexcept { return get_constants().get_minor(); }
    Symbol get_source_file_name() const noexcept { return get_constants().get_source_file_name(); }
    Symbol get_generic_signature() const noexcept { return get_constants().get_generic_signature(); }

    std::string_view get_source_debug_extension_view() const noexcept;
    int32_t get_nonstatic_field_size() const noexcept { return read_field<int32_t>(nonstatic_field_size_offset); }
    int32_t get_nonstatic_oop_map_size() const noexcept { return read_field<int32_t>(nonstatic_oop_map_size_offset); }
    int32_t get_itable_len() const noexcept { return read_field<int32_t>(itable_len_offset); }
    uint16_t get_static_oop_field_count() const noexcept { return read_field<uint16_t>(static_oop_field_count_offset); }
    uint16_t get_java_fields_count() const noexcept { return read_field<uint16_t>(java_fields_count_offset); }
    uint16_t get_all_fields_count() const noexcept;
    bool get_is_marked_dependent() const noexcept { return read_field<bool>(is_marked_dependent_offset); }

    utilities::MethodArray get_methods() const noexcept { return read_field<uint64_t>(methods_offset); }
    utilities::MethodArray get_default_methods() const noexcept { return read_field<uint64_t>(default_methods_offset); }

    utilities::KlassArray get_local_interfaces() const noexcept;
    utilities::KlassArray get_transitive_interfaces() const noexcept;

    int32_t get_size_helper() const noexcept { return get_layout_helper() / sizeof(void *); }

    Method find_method(std::string_view name, std::string_view sig) const noexcept;

    utilities::U2Array get_inner_classes() const noexcept { return read_field<uint64_t>(inner_classes_offset); }
    utilities::IntArray get_method_ordering() const noexcept { return read_field<uint64_t>(method_ordering_offset); }
    utilities::U2Array get_fields() const noexcept { return read_field<uint64_t>(fields_offset); }

    static uint64_t get_header_size() noexcept { return header_size; }

  private:
    static Method find_method(utilities::MethodArray methods, std::string_view name,
                              std::string_view signature) noexcept;

    DECLARE_STATIC_INIT

    static inline uint64_t array_klasses_offset;
    static inline uint64_t constants_offset;
    static inline uint64_t inner_classes_offset;
    static inline uint64_t source_debug_extension_offset;
    static inline uint64_t nonstatic_field_size_offset;
    static inline uint64_t static_field_size_offset;
    static inline uint64_t nonstatic_oop_map_size_offset;
    static inline uint64_t itable_len_offset;
    static inline uint64_t static_oop_field_count_offset;
    static inline uint64_t java_fields_count_offset;
    static inline uint64_t is_marked_dependent_offset;
    static inline uint64_t init_state_offset;
    static inline uint64_t misc_flags_offset;
    static inline uint64_t methods_offset;
    static inline uint64_t default_methods_offset;
    static inline uint64_t local_interfaces_offset;
    static inline uint64_t transitive_interfaces_offset;
    static inline uint64_t method_ordering_offset;
    static inline uint64_t fields_offset;

    static inline std::optional<uint64_t> breakpoints_offset;

    static inline uint64_t header_size;

    static inline int32_t ACCESS_FLAGS_OFFSET;
    static inline int32_t NAME_INDEX_OFFSET;
    static inline int32_t SIGNATURE_INDEX_OFFSET;
    static inline int32_t INITVAL_INDEX_OFFSET;
    static inline int32_t LOW_OFFSET;
    static inline int32_t HIGH_OFFSET;
    static inline int32_t FIELD_SLOTS;
    static inline int32_t FIELDINFO_TAG_SIZE;
    static inline int32_t FIELDINFO_TAG_OFFSET;

    static inline int32_t CLASS_STATE_ALLOCATED;
    static inline int32_t CLASS_STATE_LOADED;
    static inline int32_t CLASS_STATE_LINKED;
    static inline int32_t CLASS_STATE_BEING_INITIALIZED;
    static inline int32_t CLASS_STATE_FULLY_INITIALIZED;
    static inline int32_t CLASS_STATE_INITIALIZATION_ERROR;

    static inline int32_t MISC_REWRITTEN;
    static inline int32_t MISC_HAS_NONSTATIC_FIELDS;
    static inline int32_t MISC_SHOULD_VERIFY_CLASS;
    static inline int32_t MISC_IS_CONTENDED;
    static inline int32_t MISC_HAS_NONSTATIC_CONCRETE_METHODS;
    static inline int32_t MISC_DECLARES_NONSTATIC_CONCRETE_METHODS;
    static inline int32_t MISC_HAS_BEEN_REDEFINED;
    static inline int32_t MISC_IS_SCRATCH_CLASS;
    static inline int32_t MISC_IS_SHARED_BOOT_CLASS;
    static inline int32_t MISC_IS_SHARED_PLATFORM_CLASS;
    static inline int32_t MISC_IS_SHARED_APP_CLASS;
};
} // namespace hotspot::oops