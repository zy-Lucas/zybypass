#include "basicType.hpp"
#include "jvm.hpp"

namespace hotspot::runtime
{
void BasicType::initialize()
{
    T_BOOLEAN = *Jvm::lookup_int_constant("T_BOOLEAN");
    T_CHAR = *Jvm::lookup_int_constant("T_CHAR");
    T_FLOAT = *Jvm::lookup_int_constant("T_FLOAT");
    T_DOUBLE = *Jvm::lookup_int_constant("T_DOUBLE");
    T_BYTE = *Jvm::lookup_int_constant("T_BYTE");
    T_SHORT = *Jvm::lookup_int_constant("T_SHORT");
    T_INT = *Jvm::lookup_int_constant("T_INT");
    T_LONG = *Jvm::lookup_int_constant("T_LONG");
    T_OBJECT = *Jvm::lookup_int_constant("T_OBJECT");
    T_ARRAY = *Jvm::lookup_int_constant("T_ARRAY");
    T_VOID = *Jvm::lookup_int_constant("T_VOID");
    T_ADDRESS = *Jvm::lookup_int_constant("T_ADDRESS");
    T_NARROWOOP = *Jvm::lookup_int_constant("T_NARROWOOP");
    T_METADATA = *Jvm::lookup_int_constant("T_METADATA");
    T_NARROWKLASS = *Jvm::lookup_int_constant("T_NARROWKLASS");
    T_CONFLICT = *Jvm::lookup_int_constant("T_CONFLICT");
    T_ILLEGAL = *Jvm::lookup_int_constant("T_ILLEGAL");
}
} // namespace hotspot::runtime