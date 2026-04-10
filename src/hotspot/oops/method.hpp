#pragma once

#include "../runtime/accessFlag.hpp"
#include "constMethod.hpp"
#include "metaData.hpp"
#include "methodCounter.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
class Method : public MetaData
{
  public:
    Method(uint64_t addr);

    ConstMethod get_constMethod() const noexcept { return read_field<uint64_t>(constMethod_offset); }
    ConstantPool get_constants() const noexcept { return get_constMethod().get_constants(); }
    uint64_t get_method_data() const noexcept { return read_field<uint64_t>(method_data_offset); }
    MethodCounter get_method_counters() const noexcept { return read_field<uint64_t>(method_counters_offset); }

    uint16_t get_max_stack() const noexcept { return get_constMethod().get_max_stack(); }
    uint16_t get_max_locals() const noexcept { return get_constMethod().get_max_locals(); }
    uint16_t get_size_of_parameters() const noexcept { return get_constMethod().get_size_of_parameters(); }
    uint16_t get_name_index() const noexcept { return get_constMethod().get_name_index(); }
    uint16_t get_signature_index() const noexcept { return get_constMethod().get_signature_index(); }
    uint16_t get_generic_signature_index() const noexcept { return get_constMethod().get_signature_index(); }
    int32_t get_access_flags() const noexcept { return read_field<int32_t>(access_flags_offset); }
    uint16_t get_code_size() const noexcept { return get_constMethod().get_code_size(); }
    uint32_t get_vtable_index() const noexcept { return read_field<uint32_t>(vtable_index_offset); }
    uint32_t get_invocation_counter() const noexcept { return get_method_counters().get_invocation_counter(); }
    uint32_t get_backedge_counter() const noexcept { return get_method_counters().get_backedge_counter(); }

    uint64_t get_native_method() const noexcept { return read_field<uint64_t>(code_offset); }

    runtime::AccessFlags get_access_flags_obj() const noexcept { return get_access_flags(); }

    uint8_t get_u1_at(uint32_t bci) const noexcept { return get_constMethod().get_u1_at(bci); }
    uint8_t get_opcode_at(uint32_t bci) const noexcept { return get_constMethod().get_opcode_at(bci); }
    // uint8_t getOrigBytecodeAt

    uint16_t get_java_u2_at(uint32_t bci) const noexcept { return get_constMethod().get_java_u2_at(bci); }
    uint16_t get_native_u2_at(uint32_t bci) const noexcept { return get_constMethod().get_native_u2_at(bci); }

    uint16_t get_java_u4_at(uint32_t bci) const noexcept { return get_constMethod().get_java_u4_at(bci); }
    uint16_t get_native_u4_at(uint32_t bci) const noexcept { return get_constMethod().get_native_u4_at(bci); }

    Symbol get_name() const noexcept { return get_constants().get_symbol_at(get_name_index()); }
    Symbol get_signature() const noexcept { return get_constants().get_symbol_at(get_signature_index()); }
    Symbol get_generic_signature() const noexcept;

    bool is_public() const noexcept { return get_access_flags_obj().is_public(); }
    bool is_private() const noexcept { return get_access_flags_obj().is_private(); }
    bool is_protected() const noexcept { return get_access_flags_obj().is_protected(); }
    bool is_package_private() const noexcept;
    bool is_static() const noexcept { return get_access_flags_obj().is_static(); }
    bool is_final() const noexcept { return get_access_flags_obj().is_final(); }
    bool is_synchronized() const noexcept { return get_access_flags_obj().is_synchronized(); }
    bool is_bridge() const noexcept { return get_access_flags_obj().is_bridge(); }
    bool is_varargs() const noexcept { return get_access_flags_obj().is_varargs(); }
    bool is_native() const noexcept { return get_access_flags_obj().is_native(); }
    bool is_abstract() const noexcept { return get_access_flags_obj().is_abstract(); }
    bool is_strict() const noexcept { return get_access_flags_obj().is_strict(); }
    bool is_synthetic() const noexcept { return get_access_flags_obj().is_synthetic(); }

    bool is_cnstructor() const noexcept { return !is_static() && get_name().equals(object_initializer_name()); }
    bool is_static_initializer() const noexcept { return is_static() && get_name().equals(class_initializer_name()); }

    bool is_obsolete() const noexcept { return get_access_flags_obj().is_obsolete(); }

    uint32_t get_size() const noexcept { return type->get_size() + (is_native() ? 2 : 0); }

  private:
    static constexpr std::string_view object_initializer_name() noexcept { return "<init>"; }
    static constexpr std::string_view class_initializer_name() noexcept { return "<clinit>"; }

    DECLARE_STATIC_INIT

    static inline types::Type *type;

    static inline uint64_t constMethod_offset;
    static inline uint64_t method_data_offset;
    static inline uint64_t method_counters_offset;
    static inline uint64_t access_flags_offset;
    static inline uint64_t vtable_index_offset;
    static inline uint64_t code_offset;
};
} // namespace hotspot::oops