#pragma once

#include <cstdint>

namespace hotspot::runtime
{
// --- 常量池类型 ---
inline constexpr int32_t JVM_CONSTANT_Utf8 = 1;
inline constexpr int32_t JVM_CONSTANT_Unicode = 2; // 已废弃
inline constexpr int32_t JVM_CONSTANT_Integer = 3;
inline constexpr int32_t JVM_CONSTANT_Float = 4;
inline constexpr int32_t JVM_CONSTANT_Long = 5;
inline constexpr int32_t JVM_CONSTANT_Double = 6;
inline constexpr int32_t JVM_CONSTANT_Class = 7;
inline constexpr int32_t JVM_CONSTANT_String = 8;
inline constexpr int32_t JVM_CONSTANT_Fieldref = 9;
inline constexpr int32_t JVM_CONSTANT_Methodref = 10;
inline constexpr int32_t JVM_CONSTANT_InterfaceMethodref = 11;
inline constexpr int32_t JVM_CONSTANT_NameAndType = 12;
inline constexpr int32_t JVM_CONSTANT_MethodHandle = 15; // JSR 292
inline constexpr int32_t JVM_CONSTANT_MethodType = 16;   // JSR 292
inline constexpr int32_t JVM_CONSTANT_Dynamic = 17;
inline constexpr int32_t JVM_CONSTANT_InvokeDynamic = 18;
inline constexpr int32_t JVM_CONSTANT_Module = 19;
inline constexpr int32_t JVM_CONSTANT_Package = 20;
inline constexpr int32_t JVM_CONSTANT_EXTERNAL_MAX = 20;

// --- MethodHandle 引用类型 ---
inline constexpr int32_t JVM_REF_getField = 1;
inline constexpr int32_t JVM_REF_getStatic = 2;
inline constexpr int32_t JVM_REF_putField = 3;
inline constexpr int32_t JVM_REF_putStatic = 4;
inline constexpr int32_t JVM_REF_invokeVirtual = 5;
inline constexpr int32_t JVM_REF_invokeStatic = 6;
inline constexpr int32_t JVM_REF_invokeSpecial = 7;
inline constexpr int32_t JVM_REF_newInvokeSpecial = 8;
inline constexpr int32_t JVM_REF_invokeinterface = 9;

// --- 访问标志---
inline constexpr int32_t JVM_ACC_PUBLIC = 0x0001;
inline constexpr int32_t JVM_ACC_PRIVATE = 0x0002;
inline constexpr int32_t JVM_ACC_PROTECTED = 0x0004;
inline constexpr int32_t JVM_ACC_STATIC = 0x0008;
inline constexpr int32_t JVM_ACC_FINAL = 0x0010;
inline constexpr int32_t JVM_ACC_SYNCHRONIZED = 0x0020;
inline constexpr int32_t JVM_ACC_SUPER = 0x0020;
inline constexpr int32_t JVM_ACC_VOLATILE = 0x0040;
inline constexpr int32_t JVM_ACC_BRIDGE = 0x0040;
inline constexpr int32_t JVM_ACC_TRANSIENT = 0x0080;
inline constexpr int32_t JVM_ACC_VARARGS = 0x0080;
inline constexpr int32_t JVM_ACC_NATIVE = 0x0100;
inline constexpr int32_t JVM_ACC_INTERFACE = 0x0200;
inline constexpr int32_t JVM_ACC_ABSTRACT = 0x0400;
inline constexpr int32_t JVM_ACC_STRICT = 0x0800;
inline constexpr int32_t JVM_ACC_SYNTHETIC = 0x1000;
inline constexpr int32_t JVM_ACC_ANNOTATION = 0x2000;
inline constexpr int32_t JVM_ACC_ENUM = 0x4000;
inline constexpr int32_t JVM_ACC_MODULE = 0x8000;

// 组合掩码
inline constexpr int32_t JVM_RECOGNIZED_CLASS_MODIFIERS = JVM_ACC_PUBLIC | JVM_ACC_FINAL | JVM_ACC_SUPER |
                                                          JVM_ACC_INTERFACE | JVM_ACC_ABSTRACT | JVM_ACC_ANNOTATION |
                                                          JVM_ACC_ENUM | JVM_ACC_SYNTHETIC;

inline constexpr int32_t JVM_RECOGNIZED_FIELD_MODIFIERS = JVM_ACC_PUBLIC | JVM_ACC_PRIVATE | JVM_ACC_PROTECTED |
                                                          JVM_ACC_STATIC | JVM_ACC_FINAL | JVM_ACC_VOLATILE |
                                                          JVM_ACC_TRANSIENT | JVM_ACC_ENUM | JVM_ACC_SYNTHETIC;

inline constexpr int32_t JVM_RECOGNIZED_METHOD_MODIFIERS =
    JVM_ACC_PUBLIC | JVM_ACC_PRIVATE | JVM_ACC_PROTECTED | JVM_ACC_STATIC | JVM_ACC_FINAL | JVM_ACC_SYNCHRONIZED |
    JVM_ACC_BRIDGE | JVM_ACC_VARARGS | JVM_ACC_NATIVE | JVM_ACC_ABSTRACT | JVM_ACC_STRICT | JVM_ACC_SYNTHETIC;

// --- HotSpot 内部标志---
inline constexpr int32_t JVM_ACC_WRITTEN_FLAGS = 0x00007FFF;

// Method* 内部标志
inline constexpr int32_t JVM_ACC_MONITOR_MATCH = 0x10000000;
inline constexpr int32_t JVM_ACC_HAS_MONITOR_BYTECODES = 0x20000000;
inline constexpr int32_t JVM_ACC_HAS_LOOPS = 0x40000000;
inline constexpr int32_t JVM_ACC_LOOPS_FLAG_INIT = 0x80000000;
inline constexpr int32_t JVM_ACC_QUEUED = 0x01000000;
inline constexpr int32_t JVM_ACC_NOT_C2_COMPILABLE = 0x02000000;
inline constexpr int32_t JVM_ACC_NOT_C1_COMPILABLE = 0x04000000;
inline constexpr int32_t JVM_ACC_NOT_C2_OSR_COMPILABLE = 0x08000000;
inline constexpr int32_t JVM_ACC_HAS_LINE_NUMBER_TABLE = 0x00100000;
inline constexpr int32_t JVM_ACC_HAS_CHECKED_EXCEPTIONS = 0x00400000;
inline constexpr int32_t JVM_ACC_HAS_JSRS = 0x00800000;
inline constexpr int32_t JVM_ACC_IS_OLD = 0x00010000;
inline constexpr int32_t JVM_ACC_IS_OBSOLETE = 0x00020000;
inline constexpr int32_t JVM_ACC_IS_PREFIXED_NATIVE = 0x00040000;
inline constexpr int32_t JVM_ACC_ON_STACK = 0x00080000;
inline constexpr int32_t JVM_ACC_IS_DELETED = 0x00008000;

// Klass* 内部标志
inline constexpr int32_t JVM_ACC_HAS_MIRANDA_METHODS = 0x10000000;
inline constexpr int32_t JVM_ACC_HAS_VANILLA_CONSTRUCTOR = 0x20000000;
inline constexpr int32_t JVM_ACC_HAS_FINALIZER = 0x40000000;
inline constexpr int32_t JVM_ACC_IS_CLONEABLE_FAST = 0x80000000;
inline constexpr int32_t JVM_ACC_HAS_FINAL_METHOD = 0x01000000;
inline constexpr int32_t JVM_ACC_IS_SHARED_CLASS = 0x02000000;
inline constexpr int32_t JVM_ACC_IS_HIDDEN_CLASS = 0x04000000;
inline constexpr int32_t JVM_ACC_IS_VALUE_BASED_CLASS = 0x08000000;
inline constexpr int32_t JVM_ACC_IS_BEING_REDEFINED = 0x00100000;

// 共享标志
inline constexpr int32_t JVM_ACC_HAS_LOCAL_VARIABLE_TABLE = 0x00200000;
inline constexpr int32_t JVM_ACC_PROMOTED_FLAGS = 0x00200000;

// Field 内部标志
inline constexpr int32_t JVM_ACC_FIELD_ACCESS_WATCHED = 0x00002000;
inline constexpr int32_t JVM_ACC_FIELD_MODIFICATION_WATCHED = 0x00008000;
inline constexpr int32_t JVM_ACC_FIELD_INTERNAL = 0x00000400;
inline constexpr int32_t JVM_ACC_FIELD_STABLE = 0x00000020;
inline constexpr int32_t JVM_ACC_FIELD_INITIALIZED_FINAL_UPDATE = 0x00000100;
inline constexpr int32_t JVM_ACC_FIELD_HAS_GENERIC_SIGNATURE = 0x00000800;

inline constexpr int32_t JVM_ACC_FIELD_INTERNAL_FLAGS = JVM_ACC_FIELD_ACCESS_WATCHED |
                                                        JVM_ACC_FIELD_MODIFICATION_WATCHED | JVM_ACC_FIELD_INTERNAL |
                                                        JVM_ACC_FIELD_STABLE | JVM_ACC_FIELD_HAS_GENERIC_SIGNATURE;

inline constexpr int32_t JVM_ACC_FIELD_FLAGS = JVM_RECOGNIZED_FIELD_MODIFIERS | JVM_ACC_FIELD_INTERNAL_FLAGS;

} // namespace hotspot::runtime