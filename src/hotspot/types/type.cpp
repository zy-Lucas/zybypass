#include "type.hpp"
#include "field.hpp"

namespace hotspot::types
{
Type::Type(std::string_view name, Type *super_class, size_t size, bool is_oop_type, bool is_int, bool is_unsigned,
           bool is_pointer, Type *target_type)
    : name(name), super_class(super_class), size(size), is_oop_type(is_oop_type), is_int(is_int),
      is_unsigned(is_unsigned), is_pointer(is_pointer), target_type(target_type)
{
}

const Field *Type::get_field(std::string_view field_name) const noexcept
{
    auto it = name_to_field.find(field_name);
    return it != name_to_field.end() ? it->second.get() : nullptr;
}

const std::optional<uint64_t> Type::get_field_offset(std::string_view field_name) const noexcept
{
    auto field = get_field(field_name);
    return field ? std::make_optional(field->get_offset()) : std::nullopt;
}

bool Type::add_field(std::unique_ptr<Field> field)
{
    if (!field)
        return false;
    auto [it, inserted] = name_to_field.try_emplace(field->get_field_name(), std::move(field));
    return inserted;
}

inline std::ostream &operator<<(std::ostream &os, const Type &type)
{
    os << "Type{name='" << type.name << "', super_class_name='" << type.super_class->get_name() << "', size=" << type.size
       << ", is_oop_type=" << type.is_oop_type << ", is_int=" << type.is_int << ", is_unsigned=" << type.is_unsigned
       << ", is_pointer=" << type.is_pointer << ", fields=[";

    bool first = true;
    for (const auto &pair : type.name_to_field) // 乱序但是无伤大雅
    {
        if (!first)
            os << ", ";
        first = false;
        os << "{type_name='" << pair.second->get_type_name() << "', field_name='" << pair.first << "'}";
    }

    os << "]}";
    return os;
}
} // namespace hotspot::types