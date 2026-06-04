#include "fieldResolver.hpp"

namespace hotspot::utils::constants
{
void int_const(std::string_view name, int32_t &dest) noexcept
{
    dest = *runtime::Jvm::lookup_int_constant(name);
}

void long_const(std::string_view name, int64_t &dest) noexcept
{
    dest = *runtime::Jvm::lookup_long_constant(name);
}
} // namespace hotspot::utils