#include "typeArrayKlass.hpp"

namespace hotspot::oops
{
std::string_view TypeArrayKlass::get_type_name() const noexcept
{
    static constexpr std::array<std::string_view, 8> names{"[Z", "[C", "[F", "[D", "[B", "[S", "[I", "[J"};
    if (uint32_t t = get_element_type(); t >= T_BOOLEAN && t <= T_LONG)
        return names[t - T_BOOLEAN];
    return "Unknown TypeArray";
}

std::string_view TypeArrayKlass::get_element_eype_name() const noexcept
{
    static constexpr std::array<std::string_view, 8> names{"boolean", "char",  "float", "double",
                                                           "byte",    "short", "int",   "long"};
    if (uint32_t t = get_element_type(); t >= T_BOOLEAN && t <= T_LONG)
        return names[t - T_BOOLEAN];
    return "unknown";
}

void TypeArrayKlass::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("TypeArrayKlass");

    max_length_offset = *type->get_field_offset("_max_length");
}
} // namespace hotspot::oops