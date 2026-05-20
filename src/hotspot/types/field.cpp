#include "field.hpp"
#include "type.hpp"

namespace hotspot::types
{
Field::Field(std::string_view type_name, std::string_view field_name, Type *field_type, bool is_static, uint64_t offset) noexcept
    : type_name_(type_name), field_name_(field_name), field_type_(field_type), is_static_(is_static), offset_(offset)
{
}

std::ostream &operator<<(std::ostream &os, const Field &field)
{
    os << "Field{type_name='" << field.type_name_ << "', field_name='" << field.field_name_
       << "', is_static=" << field.is_static_ << ", offset=" << field.offset_
       << ", field_type=" << (field.field_type_ ? field.field_type_->name() : "null") << "}";
    return os;
}
} // namespace hotspot::types