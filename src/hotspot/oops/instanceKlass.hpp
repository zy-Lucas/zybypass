#pragma once

#include "code/nmethodBucket.hpp"
#include "constantPool.hpp"
#include "fieldType.hpp"
#include "klass.hpp"

namespace hotspot::oops
{
class Field;
class Oop;
class klassItable;

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

    Value value_;

    constexpr ClassState(Value v) noexcept : value_(v) {}
    constexpr ClassState(uint8_t v) noexcept : value_(Value(v)) {}

    constexpr uint8_t raw() const noexcept { return value_; }
    constexpr std::string_view to_string() const noexcept;

    constexpr bool is_loaded() const noexcept { return value_ >= Value::loaded; }
    constexpr bool is_linked() const noexcept { return value_ >= Value::linked; }
    constexpr bool is_initialized() const noexcept { return value_ == Value::fully_initialized; }
    constexpr bool is_not_initialized() const noexcept { return value_ < Value::being_initialized; }
    constexpr bool is_being_initialized() const noexcept { return value_ == Value::being_initialized; }
    constexpr bool is_in_error_state() const noexcept { return value_ == Value::initialization_error; }

    constexpr bool operator==(const ClassState &) const noexcept = default;
    constexpr auto operator<=>(const ClassState &) const noexcept = default;
};

class InstanceKlass : public Klass
{
  public:
    InstanceKlass(uint64_t addr) noexcept : Klass(addr) {}
    InstanceKlass(Klass klass) noexcept : Klass(klass) {}

    uint64_t size() const noexcept;

    uint16_t field_access_flags(uint32_t index) const noexcept;
    Symbol field_name(uint32_t index) const noexcept;
    Symbol field_signature(uint32_t index) const noexcept;
    uint16_t field_generic_signature_index(uint32_t index) const noexcept;
    Symbol field_generic_signature(uint32_t index) const noexcept;
    uint16_t field_initial_value_index(uint32_t index) const noexcept;
    uint32_t field_offset(uint32_t index) const noexcept;

    ConstantPool constants() const noexcept { return read_field<uint64_t>(constants_offset_); }

    uint16_t major_version() const noexcept { return constants().major(); }
    uint16_t minor_version() const noexcept { return constants().minor(); }
    Symbol source_file_name() const noexcept { return constants().source_file_name(); }
    Symbol generic_signature() const noexcept { return constants().generic_signature(); }

    std::string_view source_debug_extension_view() const noexcept;
    std::string source_debug_extension() const { return std::string{source_debug_extension_view()}; }
    int32_t nonstatic_field_size() const noexcept { return read_field<int32_t>(nonstatic_field_size_offset_); }
    int32_t nonstatic_oop_map_size() const noexcept { return read_field<int32_t>(nonstatic_oop_map_size_offset_); }
    int32_t itable_length() const noexcept { return read_field<int32_t>(itable_len_offset_); }
    uint16_t static_oop_field_count() const noexcept { return read_field<uint16_t>(static_oop_field_count_offset_); }
    uint16_t java_fields_count() const noexcept { return read_field<uint16_t>(java_fields_count_offset_); }
    uint16_t all_fields_count() const noexcept;
    bool is_marked_dependent() const noexcept { return read_field<bool>(is_marked_dependent_offset_); }

    uint16_t method_idnum() const noexcept { return read_field<uint16_t>(idnum_allocated_count_offset_); }
    uint16_t next_method_idnum() noexcept;

    ClassState init_state() const noexcept { return read_field<uint8_t>(init_state_offset_); }

    bool is_loaded() const noexcept { return init_state().is_loaded(); }
    bool is_linked() const noexcept { return init_state().is_linked(); }
    bool is_initialized() const noexcept { return init_state().is_initialized(); }
    bool is_not_initialized() const noexcept { return init_state().is_not_initialized(); }
    bool is_being_initialized() const noexcept { return init_state().is_being_initialized(); }
    bool is_in_error_state() const noexcept { return init_state().is_in_error_state(); }

    uint64_t *methods_jmethod_ids() const noexcept;

    uint64_t jmethod_id_or_null(Method method) const noexcept;

    code::nmethodBucket dep_context() const noexcept { return read_field<uint64_t>(dep_context_offset_); }

    utilities::MethodArray methods() const noexcept { return read_field<uint64_t>(methods_offset_); }
    utilities::MethodArray default_methods() const noexcept { return read_field<uint64_t>(default_methods_offset_); }

    void adjust_default_method(Method old_method, Method new_method) noexcept;

    utilities::KlassArray local_interfaces() const noexcept;
    utilities::KlassArray transitive_interfaces() const noexcept;

    uint64_t start_of_itable() const noexcept { return start_of_vtable() + vtable_length() * 8; }
    uint64_t end_of_itable() const noexcept { return start_of_itable() + itable_length() * 8; }

    klassItable itable() const noexcept;

    int32_t size_helper() const noexcept { return layout_helper() / sizeof(void *); }

    Method find_method(std::string_view name, std::string_view sig) const noexcept;
    int32_t find_method_index(std::string_view name, std::string_view sig) const noexcept;

    utilities::U2Array inner_classes() const noexcept { return read_field<uint64_t>(inner_classes_offset_); }
    utilities::IntArray method_ordering() const noexcept { return read_field<uint64_t>(method_ordering_offset_); }
    utilities::U2Array fields() const noexcept { return read_field<uint64_t>(fields_offset_); }

    template <typename Visitor> void iterate_static_fields(Visitor &&visitor);
    template <typename Visitor> void iterate_non_static_fields(Visitor &&visitor, const Oop &obj);

    Field find_field(std::string_view name, std::string_view sig) const noexcept;
    Field find_local_field(std::string_view name, std::string_view sig) const noexcept;
    Field find_interface_field(std::string_view name, std::string_view sig) const noexcept;

    static uint64_t header_size() noexcept { return header_size_; }

  private:
    template <typename Visitor> void visit_field(Visitor &&visitor, FieldType type, uint32_t index);

    static int32_t linear_search(utilities::MethodArray methods, std::string_view name,
                                 std::string_view signature) noexcept;

    DECLARE_STATIC_INIT

    static inline uint64_t array_klasses_offset_;
    static inline uint64_t constants_offset_;
    static inline uint64_t inner_classes_offset_;
    static inline uint64_t source_debug_extension_offset_;
    static inline uint64_t nonstatic_field_size_offset_;
    static inline uint64_t static_field_size_offset_;
    static inline uint64_t nonstatic_oop_map_size_offset_;
    static inline uint64_t itable_len_offset_;
    static inline uint64_t static_oop_field_count_offset_;
    static inline uint64_t java_fields_count_offset_;
    static inline uint64_t is_marked_dependent_offset_;
    static inline uint64_t idnum_allocated_count_offset_;
    static inline uint64_t init_state_offset_;
    static inline uint64_t misc_flags_offset_;
    static inline uint64_t methods_jmethod_ids_offset_;
    static inline uint64_t dep_context_offset_;
    static inline uint64_t methods_offset_;
    static inline uint64_t default_methods_offset_;
    static inline uint64_t local_interfaces_offset_;
    static inline uint64_t transitive_interfaces_offset_;
    static inline uint64_t method_ordering_offset_;
    static inline uint64_t fields_offset_;

    static inline std::optional<uint64_t> breakpoints_offset_;

    static inline uint64_t header_size_;

    static inline int32_t access_flags_offset_;
    static inline int32_t name_index_offset_;
    static inline int32_t signature_index_offset_;
    static inline int32_t initval_index_offset_;
    static inline int32_t low_offset_;
    static inline int32_t high_offset_;
    static inline int32_t field_slots_;
    static inline int32_t field_info_tag_size_;
    static inline int32_t field_info_tag_offset_;

    static inline int32_t class_state_allocated_;
    static inline int32_t class_state_loaded_;
    static inline int32_t class_state_linked_;
    static inline int32_t class_state_being_initialized_;
    static inline int32_t class_state_fully_initialized_;
    static inline int32_t class_state_initialization_error_;

    static inline int32_t misc_rewritten_;
    static inline int32_t misc_has_nonstatic_fields_;
    static inline int32_t misc_should_verify_class_;
    static inline int32_t misc_is_contended_;
    static inline int32_t misc_has_nonstatic_concrete_methods_;
    static inline int32_t misc_declares_nonstatic_concrete_methods_;
    static inline int32_t misc_has_beed_redefined_;
    static inline int32_t misc_is_scratch_class_;
    static inline int32_t misc_is_shared_boot_class_;
    static inline int32_t misc_is_shared_platform_class_;
    static inline int32_t misc_is_shared_app_class_;
};
} // namespace hotspot::oops