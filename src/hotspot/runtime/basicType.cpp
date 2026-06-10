#include "basicType.hpp"
#include "jvm.hpp"

namespace hotspot::runtime
{
void BasicType::initialize()
{
    utils::constants::int_const("T_BOOLEAN", T_BOOLEAN);
    utils::constants::int_const("T_CHAR", T_CHAR);
    utils::constants::int_const("T_FLOAT", T_FLOAT);
    utils::constants::int_const("T_DOUBLE", T_DOUBLE);
    utils::constants::int_const("T_BYTE", T_BYTE);
    utils::constants::int_const("T_SHORT", T_SHORT);
    utils::constants::int_const("T_INT", T_INT);
    utils::constants::int_const("T_LONG", T_LONG);
    utils::constants::int_const("T_OBJECT", T_OBJECT);
    utils::constants::int_const("T_ARRAY", T_ARRAY);
    utils::constants::int_const("T_VOID", T_VOID);
    utils::constants::int_const("T_ADDRESS", T_ADDRESS);
    utils::constants::int_const("T_NARROWOOP", T_NARROWOOP);
    utils::constants::int_const("T_METADATA", T_METADATA);
    utils::constants::int_const("T_NARROWKLASS", T_NARROWKLASS);
    utils::constants::int_const("T_CONFLICT", T_CONFLICT);
    utils::constants::int_const("T_ILLEGAL", T_ILLEGAL);
}
} // namespace hotspot::runtime