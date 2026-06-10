#include "typeArrayKlass.hpp"

namespace hotspot::oops
{
std::string_view TypeArrayKlass::type_name() const noexcept
{
    static constexpr std::array<std::string_view, 8> names{"[Z", "[C", "[F", "[D", "[B", "[S", "[I", "[J"};
    if (uint32_t t = element_type(); t >= T_BOOLEAN && t <= T_LONG)
        return names[t - T_BOOLEAN];
    return "Unknown TypeArray";
}

std::string_view TypeArrayKlass::element_eype_name() const noexcept
{
    static constexpr std::array<std::string_view, 8> names{"boolean", "char",  "float", "double",
                                                           "byte",    "short", "int",   "long"};
    if (uint32_t t = element_type(); t >= T_BOOLEAN && t <= T_LONG)
        return names[t - T_BOOLEAN];
    return "unknown";
}

void TypeArrayKlass::initialize()
{
    utils::FieldResolver r{"TypeArrayKlass"};

    r.field_offset("_max_length", max_length_offset_);
}
} // namespace hotspot::oops