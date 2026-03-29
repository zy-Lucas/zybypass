#pragma once

#include "classConstants.h"

namespace hotspot::runtime
{
class AccessFlags
{
  public:
    constexpr AccessFlags(int32_t flag) noexcept : flags(flag) {}

    // Java access flags
    constexpr bool is_public() const noexcept { return flags & JVM_ACC_PUBLIC; }
    constexpr bool is_private() const noexcept { return flags & JVM_ACC_PRIVATE; }
    constexpr bool is_protected() const noexcept { return flags & JVM_ACC_PROTECTED; }
    constexpr bool is_static() const noexcept { return flags & JVM_ACC_STATIC; }
    constexpr bool is_final() const noexcept { return flags & JVM_ACC_FINAL; }
    constexpr bool is_synchronized() const noexcept { return flags & JVM_ACC_SYNCHRONIZED; }
    constexpr bool is_super() const noexcept { return flags & JVM_ACC_SUPER; }
    constexpr bool is_volatile() const noexcept { return flags & JVM_ACC_VOLATILE; }
    constexpr bool is_bridge() const noexcept { return flags & JVM_ACC_BRIDGE; }
    constexpr bool is_transient() const noexcept { return flags & JVM_ACC_TRANSIENT; }
    constexpr bool is_varargs() const noexcept { return flags & JVM_ACC_VARARGS; }
    constexpr bool is_native() const noexcept { return flags & JVM_ACC_NATIVE; }
    constexpr bool is_enum() const noexcept { return flags & JVM_ACC_ENUM; }
    constexpr bool is_annotation() const noexcept { return flags & JVM_ACC_ANNOTATION; }
    constexpr bool is_interface() const noexcept { return flags & JVM_ACC_INTERFACE; }
    constexpr bool is_abstract() const noexcept { return flags & JVM_ACC_ABSTRACT; }
    constexpr bool is_strict() const noexcept { return flags & JVM_ACC_STRICT; }
    constexpr bool is_synthetic() const noexcept { return flags & JVM_ACC_SYNTHETIC; }

    // Method* flags
    constexpr bool is_monitor_matching() const noexcept { return flags & JVM_ACC_MONITOR_MATCH; }
    constexpr bool has_monitor_bytecodes() const noexcept { return flags & JVM_ACC_HAS_MONITOR_BYTECODES; }
    constexpr bool has_loops() const noexcept { return flags & JVM_ACC_HAS_LOOPS; }
    constexpr bool is_loops_flag_init() const noexcept { return flags & JVM_ACC_LOOPS_FLAG_INIT; }
    constexpr bool is_queued_for_compilation() const noexcept { return flags & JVM_ACC_QUEUED; }
    constexpr bool is_not_osr_compilable() const noexcept { return flags & JVM_ACC_NOT_C2_OSR_COMPILABLE; }
    constexpr bool has_line_number_table() const noexcept { return flags & JVM_ACC_HAS_LINE_NUMBER_TABLE; }
    constexpr bool has_checked_exceptions() const noexcept { return flags & JVM_ACC_HAS_CHECKED_EXCEPTIONS; }
    constexpr bool has_jsrs() const noexcept { return flags & JVM_ACC_HAS_JSRS; }
    constexpr bool is_obsolete() const noexcept { return flags & JVM_ACC_IS_OBSOLETE; }

    // Klass* flags
    constexpr bool has_miranda_methods() const noexcept { return flags & JVM_ACC_HAS_MIRANDA_METHODS; }
    constexpr bool has_vanilla_constructor() const noexcept { return flags & JVM_ACC_HAS_VANILLA_CONSTRUCTOR; }
    constexpr bool has_finalizer() const noexcept { return flags & JVM_ACC_HAS_FINALIZER; }
    constexpr bool is_cloneable() const noexcept { return flags & JVM_ACC_IS_CLONEABLE_FAST; }

    // Klass* and Method* shared flags
    constexpr bool has_local_variable_table() const noexcept { return flags & JVM_ACC_HAS_LOCAL_VARIABLE_TABLE; }

    // Field flags
    constexpr bool is_field_access_watched() const noexcept { return flags & JVM_ACC_FIELD_ACCESS_WATCHED; }
    constexpr bool is_field_modification_watched() const noexcept { return flags & JVM_ACC_FIELD_MODIFICATION_WATCHED; }
    constexpr bool field_has_generic_signature() const noexcept { return flags & JVM_ACC_FIELD_HAS_GENERIC_SIGNATURE; }

    // get flags written to .class files
    constexpr int32_t getStandardFlags() { return flags & JVM_ACC_WRITTEN_FLAGS; }

    constexpr int32_t value() const noexcept { return flags; }

  private:
    int32_t flags;
};
} // namespace hotspot::runtime