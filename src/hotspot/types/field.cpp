#include "field.hpp"
#include "type.hpp"

namespace hotspot::types
{
Field::Field(std::string_view type_name, std::string_view field_name, Type *field_type, bool is_static, uint64_t offset)
    : type_name(type_name), field_name(field_name), field_type(field_type), is_static(is_static), offset(offset)
{
}

inline std::ostream &operator<<(std::ostream &os, const Field &field)
{
    os << "Field{type_name='" << field.type_name << "', field_name='" << field.field_name
       << "', is_static=" << field.is_static << ", offset=" << field.offset
       << ", field_type=" << (field.field_type ? field.field_type->get_name() : "null") << "}";
    return os;
}
} // namespace hotspot::types