#pragma once

#include "../runtime/accessFlag.hpp"
#include "constMethod.hpp"
#include "metaData.hpp"
#include "methodCounters.hpp"

namespace hotspot::code
{
class nmethod;
}

namespace hotspot::oops
{
class ConstMethod;

class Method : public MetaData
{
  public:
    Method(uint64_t addr) : MetaData(addr) {}

    ConstMethod constMethod() const noexcept;
    ConstantPool constants() const noexcept { return constMethod().constants(); }
    uint64_t method_data() const noexcept { return read_field<uint64_t>(method_data_offset_); }
    MethodCounter method_counters() const noexcept { return read_field<uint64_t>(method_counters_offset_); }

    uint16_t max_stack() const noexcept { return constMethod().max_stack(); }
    uint16_t max_locals() const noexcept { return constMethod().max_locals(); }
    uint16_t size_of_parameters() const noexcept { return constMethod().size_of_parameters(); }
    uint16_t name_index() const noexcept { return constMethod().name_index(); }
    uint16_t signature_index() const noexcept { return constMethod().signature_index(); }
    uint16_t code_size() const noexcept { return constMethod().code_size(); }
    uint32_t vtable_index() const noexcept { return read_field<uint32_t>(vtable_index_offset_); }
    uint32_t invocation_counter() const noexcept { return method_counters().invocation_counter(); }
    uint32_t backedge_counter() const noexcept { return method_counters().backedge_counter(); }

    code::nmethod native_method() const noexcept;

    uint64_t from_interpreter_entry() const noexcept { return read_field<uint64_t>(from_interpreter_entry_offset_); }
    void set_from_interpreter_entry(uint64_t addr) noexcept;

    int32_t access_flags() const noexcept { return read_field<int32_t>(access_flags_offset_); }
    void set_access_flags(int32_t access_flags) noexcept { write_field(access_flags_offset_, access_flags); }

    runtime::AccessFlags access_flags_obj() const noexcept { return access_flags(); }

    void set_flag(int32_t flag, bool value) noexcept;

    uint8_t u1_at(uint32_t bci) const noexcept { return constMethod().u1_at(bci); }
    uint8_t opcode_at(uint32_t bci) const noexcept { return constMethod().opcode_at(bci); }
    // uint8_t getOrigBytecodeAt

    uint16_t java_u2_at(uint32_t bci) const noexcept { return constMethod().java_u2_at(bci); }
    uint16_t native_u2_at(uint32_t bci) const noexcept { return constMethod().native_u2_at(bci); }

    uint16_t java_u4_at(uint32_t bci) const noexcept { return constMethod().java_u4_at(bci); }
    uint16_t native_u4_at(uint32_t bci) const noexcept { return constMethod().native_u4_at(bci); }

    Symbol name() const noexcept { return constants().symbol_at(name_index()); }
    Symbol signature() const noexcept { return constants().symbol_at(signature_index()); }

    bool is_public() const noexcept { return access_flags_obj().is_public(); }
    void set_is_public(bool value) noexcept { set_flag(runtime::JVM_ACC_PUBLIC, value); }

    bool is_private() const noexcept { return access_flags_obj().is_private(); }
    void set_is_private(bool value) noexcept { set_flag(runtime::JVM_ACC_PRIVATE, value); }

    bool is_protected() const noexcept { return access_flags_obj().is_protected(); }
    void set_is_protected(bool value) noexcept { set_flag(runtime::JVM_ACC_PROTECTED, value); }

    bool is_package_private() const noexcept;
    void set_is_package_private(bool value) noexcept;

    bool is_static() const noexcept { return access_flags_obj().is_static(); }
    void set_is_static(bool value) noexcept { set_flag(runtime::JVM_ACC_STATIC, value); }

    bool is_final() const noexcept { return access_flags_obj().is_final(); }
    void set_is_final(bool value) noexcept { set_flag(runtime::JVM_ACC_FINAL, value); }

    bool is_synchronized() const noexcept { return access_flags_obj().is_synchronized(); }
    void set_is_synchronized(bool value) noexcept { set_flag(runtime::JVM_ACC_SYNCHRONIZED, value); }

    bool is_bridge() const noexcept { return access_flags_obj().is_bridge(); }
    void set_is_bridge(bool value) noexcept { set_flag(runtime::JVM_ACC_BRIDGE, value); }

    bool is_varargs() const noexcept { return access_flags_obj().is_varargs(); }
    void set_is_varargs(bool value) noexcept { set_flag(runtime::JVM_ACC_VARARGS, value); }

    bool is_native() const noexcept { return access_flags_obj().is_native(); }
    void set_is_native(bool value) noexcept { set_flag(runtime::JVM_ACC_NATIVE, value); }

    bool is_abstract() const noexcept { return access_flags_obj().is_abstract(); }
    void set_is_abstract(bool value) noexcept { set_flag(runtime::JVM_ACC_ABSTRACT, value); }

    bool is_strict() const noexcept { return access_flags_obj().is_strict(); }
    void set_is_strict(bool value) noexcept { set_flag(runtime::JVM_ACC_STRICT, value); }

    bool is_synthetic() const noexcept { return access_flags_obj().is_synthetic(); }
    void set_is_synthetic(bool value) noexcept { set_flag(runtime::JVM_ACC_SYNTHETIC, value); }

    bool is_obsolete() const noexcept { return access_flags_obj().is_obsolete(); }
    void set_is_obsolete(bool value) noexcept { set_flag(runtime::JVM_ACC_IS_OBSOLETE, value); }

    bool is_constructor() const noexcept { return !is_static() && name().equals("<init>"); }
    bool is_static_initializer() const noexcept { return is_static() && name().equals("<clinit>"); }

    uint64_t size() const noexcept { return type_->size() + (is_native() ? 2 * sizeof(void *) : 0); }

  private:
    DECLARE_STATIC_INIT

  public:
    static inline types::Type *type_;

    static inline uint64_t constMethod_offset_;
    static inline uint64_t method_data_offset_;
    static inline uint64_t method_counters_offset_;
    static inline uint64_t access_flags_offset_;
    static inline uint64_t vtable_index_offset_;
    static inline uint64_t code_offset_;
    static inline uint64_t from_interpreter_entry_offset_;
};
} // namespace hotspot::oops