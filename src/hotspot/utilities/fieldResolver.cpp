#include "fieldResolver.hpp"

namespace hotspot::utils
{
void FieldResolver::field_offset(std::string_view name, std::optional<uint64_t> &dest) const noexcept
{
    dest = type_->field_offset(name);
}
} // namespace hotspot::utils