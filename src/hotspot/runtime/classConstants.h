#pragma once

#define JVM_RECOGNIZED_CLASS_MODIFIERS                                                                                 \
    (JVM_ACC_PUBLIC | JVM_ACC_FINAL | JVM_ACC_SUPER | JVM_ACC_INTERFACE | JVM_ACC_ABSTRACT | JVM_ACC_ANNOTATION |      \
     JVM_ACC_ENUM | JVM_ACC_SYNTHETIC)

#define JVM_RECOGNIZED_FIELD_MODIFIERS                                                                                 \
    (JVM_ACC_PUBLIC | JVM_ACC_PRIVATE | JVM_ACC_PROTECTED | JVM_ACC_STATIC | JVM_ACC_FINAL | JVM_ACC_VOLATILE |        \
     JVM_ACC_TRANSIENT | JVM_ACC_ENUM | JVM_ACC_SYNTHETIC)

#define JVM_RECOGNIZED_METHOD_MODIFIERS                                                                                \
    (JVM_ACC_PUBLIC | JVM_ACC_PRIVATE | JVM_ACC_PROTECTED | JVM_ACC_STATIC | JVM_ACC_FINAL | JVM_ACC_SYNCHRONIZED |    \
     JVM_ACC_BRIDGE | JVM_ACC_VARARGS | JVM_ACC_NATIVE | JVM_ACC_ABSTRACT | JVM_ACC_STRICT | JVM_ACC_SYNTHETIC)

enum
{
    JVM_CONSTANT_Utf8 = 1,
    JVM_CONSTANT_Unicode = 2, /* unused */
    JVM_CONSTANT_Integer = 3,
    JVM_CONSTANT_Float = 4,
    JVM_CONSTANT_Long = 5,
    JVM_CONSTANT_Double = 6,
    JVM_CONSTANT_Class = 7,
    JVM_CONSTANT_String = 8,
    JVM_CONSTANT_Fieldref = 9,
    JVM_CONSTANT_Methodref = 10,
    JVM_CONSTANT_InterfaceMethodref = 11,
    JVM_CONSTANT_NameAndType = 12,
    JVM_CONSTANT_MethodHandle = 15, // JSR 292
    JVM_CONSTANT_MethodType = 16,   // JSR 292
    JVM_CONSTANT_Dynamic = 17,
    JVM_CONSTANT_InvokeDynamic = 18,
    JVM_CONSTANT_Module = 19,
    JVM_CONSTANT_Package = 20,
    JVM_CONSTANT_ExternalMax = 20
};

enum
{
    JVM_REF_getField = 1,
    JVM_REF_getStatic = 2,
    JVM_REF_putField = 3,
    JVM_REF_putStatic = 4,
    JVM_REF_invokeVirtual = 5,
    JVM_REF_invokeStatic = 6,
    JVM_REF_invokeSpecial = 7,
    JVM_REF_newInvokeSpecial = 8,
    JVM_REF_invokeInterface = 9
};

enum
{
    // See jvm.h for shared JVM_CONSTANT_XXX tags
    // NOTE: replicated in SA in vm/agent/sun/jvm/hotspot/utilities/ConstantTag.java
    // Hotspot specific tags
    JVM_CONSTANT_Invalid = 0,                  // For bad value initialization
    JVM_CONSTANT_InternalMin = 100,            // First implementation tag (aside from bad value of course)
    JVM_CONSTANT_UnresolvedClass = 100,        // Temporary tag until actual use
    JVM_CONSTANT_ClassIndex = 101,             // Temporary tag while constructing constant pool
    JVM_CONSTANT_StringIndex = 102,            // Temporary tag while constructing constant pool
    JVM_CONSTANT_UnresolvedClassInError = 103, // Error tag due to resolution error
    JVM_CONSTANT_MethodHandleInError = 104,    // Error tag due to resolution error
    JVM_CONSTANT_MethodTypeInError = 105,      // Error tag due to resolution error
    JVM_CONSTANT_DynamicInError = 106,         // Error tag due to resolution error
    JVM_CONSTANT_InternalMax = 106             // Last implementation tag
};

enum
{
    JVM_ACC_PUBLIC = 0x0001,
    JVM_ACC_PRIVATE = 0x0002,
    JVM_ACC_PROTECTED = 0x0004,
    JVM_ACC_STATIC = 0x0008,
    JVM_ACC_FINAL = 0x0010,
    JVM_ACC_SYNCHRONIZED = 0x0020,
    JVM_ACC_SUPER = 0x0020,
    JVM_ACC_VOLATILE = 0x0040,
    JVM_ACC_BRIDGE = 0x0040,
    JVM_ACC_TRANSIENT = 0x0080,
    JVM_ACC_VARARGS = 0x0080,
    JVM_ACC_NATIVE = 0x0100,
    JVM_ACC_INTERFACE = 0x0200,
    JVM_ACC_ABSTRACT = 0x0400,
    JVM_ACC_STRICT = 0x0800,
    JVM_ACC_SYNTHETIC = 0x1000,
    JVM_ACC_ANNOTATION = 0x2000,
    JVM_ACC_ENUM = 0x4000,
    JVM_ACC_MODULE = 0x8000
};

enum
{
    // See jvm.h for shared JVM_ACC_XXX access flags

    // HotSpot-specific access flags

    // flags actually put in .class file
    JVM_ACC_WRITTEN_FLAGS = 0x00007FFF,

    // Method* flags
    JVM_ACC_MONITOR_MATCH = 0x10000000,         // True if we know that monitorenter/monitorexit bytecodes match
    JVM_ACC_HAS_MONITOR_BYTECODES = 0x20000000, // Method contains monitorenter/monitorexit bytecodes
    JVM_ACC_HAS_LOOPS = 0x40000000,             // Method has loops
    JVM_ACC_LOOPS_FLAG_INIT = (int)0x80000000,  // The loop flag has been initialized
    JVM_ACC_QUEUED = 0x01000000,                // Queued for compilation
    JVM_ACC_NOT_C2_COMPILABLE = 0x02000000,
    JVM_ACC_NOT_C1_COMPILABLE = 0x04000000,
    JVM_ACC_NOT_C2_OSR_COMPILABLE = 0x08000000,
    JVM_ACC_HAS_LINE_NUMBER_TABLE = 0x00100000,
    JVM_ACC_HAS_CHECKED_EXCEPTIONS = 0x00400000,
    JVM_ACC_HAS_JSRS = 0x00800000,
    JVM_ACC_IS_OLD = 0x00010000,             // RedefineClasses() has replaced this method
    JVM_ACC_IS_OBSOLETE = 0x00020000,        // RedefineClasses() has made method obsolete
    JVM_ACC_IS_PREFIXED_NATIVE = 0x00040000, // JVMTI has prefixed this native method
    JVM_ACC_ON_STACK = 0x00080000,           // RedefineClasses() was used on the stack
    JVM_ACC_IS_DELETED = 0x00008000,         // RedefineClasses() has deleted this method

    // Klass* flags
    JVM_ACC_HAS_MIRANDA_METHODS = 0x10000000,     // True if this class has miranda methods in it's vtable
    JVM_ACC_HAS_VANILLA_CONSTRUCTOR = 0x20000000, // True if klass has a vanilla default constructor
    JVM_ACC_HAS_FINALIZER = 0x40000000,           // True if klass has a non-empty finalize() method
    JVM_ACC_IS_CLONEABLE_FAST =
        (int)0x80000000, // True if klass implements the Cloneable interface and can be optimized in generated code
    JVM_ACC_HAS_FINAL_METHOD = 0x01000000,     // True if klass has final method
    JVM_ACC_IS_SHARED_CLASS = 0x02000000,      // True if klass is shared
    JVM_ACC_IS_HIDDEN_CLASS = 0x04000000,      // True if klass is hidden
    JVM_ACC_IS_VALUE_BASED_CLASS = 0x08000000, // True if klass is marked as a ValueBased class
    JVM_ACC_IS_BEING_REDEFINED = 0x00100000,   // True if the klass is being redefined.

    // Klass* and Method* flags
    JVM_ACC_HAS_LOCAL_VARIABLE_TABLE = 0x00200000,

    JVM_ACC_PROMOTED_FLAGS = 0x00200000, // flags promoted from methods to the holding klass

    // field flags
    // Note: these flags must be defined in the low order 16 bits because
    // InstanceKlass only stores a ushort worth of information from the
    // AccessFlags value.
    // These bits must not conflict with any other field-related access flags
    // (e.g., ACC_ENUM).
    // Note that the class-related ACC_ANNOTATION bit conflicts with these flags.
    JVM_ACC_FIELD_ACCESS_WATCHED = 0x00002000,       // field access is watched by JVMTI
    JVM_ACC_FIELD_MODIFICATION_WATCHED = 0x00008000, // field modification is watched by JVMTI
    JVM_ACC_FIELD_INTERNAL = 0x00000400,             // internal field, same as JVM_ACC_ABSTRACT
    JVM_ACC_FIELD_STABLE = 0x00000020,               // @Stable field, same as JVM_ACC_SYNCHRONIZED and JVM_ACC_SUPER
    JVM_ACC_FIELD_INITIALIZED_FINAL_UPDATE =
        0x00000100, // (static) final field updated outside (class) initializer, same as JVM_ACC_NATIVE
    JVM_ACC_FIELD_HAS_GENERIC_SIGNATURE = 0x00000800, // field has generic signature

    JVM_ACC_FIELD_INTERNAL_FLAGS = JVM_ACC_FIELD_ACCESS_WATCHED | JVM_ACC_FIELD_MODIFICATION_WATCHED |
                                   JVM_ACC_FIELD_INTERNAL | JVM_ACC_FIELD_STABLE | JVM_ACC_FIELD_HAS_GENERIC_SIGNATURE,

    // flags accepted by set_field_flags()
    JVM_ACC_FIELD_FLAGS = JVM_RECOGNIZED_FIELD_MODIFIERS | JVM_ACC_FIELD_INTERNAL_FLAGS
};