#include "type.hpp"
#include "field.hpp"

namespace hotspot::types
{
Type::Type(std::string_view name, Type *super_class, uint64_t size, bool is_oop_type, bool is_int, bool is_unsigned,
           bool is_pointer, Type *target_type) noexcept
    : name_(name), super_class_(super_class), size_(size), is_oop_type_(is_oop_type), is_int_(is_int),
      is_unsigned_(is_unsigned), is_pointer_(is_pointer), target_type_(target_type)
{
}

const Field *Type::lookup_field(std::string_view field_name) const noexcept
{
    if (auto it = name_to_field_.find(field_name); it != name_to_field_.end())
        return it->second.get();
    return nullptr;
}

std::optional<uint64_t> Type::field_offset(std::string_view field_name) const noexcept
{
    if (const Field *field = lookup_field(field_name); field)
        return field->offset();
    return std::nullopt;
}

void Type::set_is_pointer(bool new_is_pointer, types::Type *new_target_type) noexcept
{
    is_pointer_ = new_is_pointer;
    target_type_ = new_target_type;
}

bool Type::add_field(std::unique_ptr<Field> field)
{
    if (!field)
        return false;
    auto [it, inserted] = name_to_field_.try_emplace(field->field_name(), std::move(field));
    return inserted;
}

std::ostream &operator<<(std::ostream &os, const Type &type)
{
    os << "Type{name='" << type.name_ << "', super_class_name='" << type.super_class_->name()
       << "', size=" << type.size_ << ", is_oop_type=" << type.is_oop_type_ << ", is_int=" << type.is_int_
       << ", is_unsigned=" << type.is_unsigned_ << ", is_pointer=" << type.is_pointer_ << ", fields=[";

    bool first = true;
    for (const auto &pair : type.name_to_field_)
    {
        if (!first)
            os << ", ";
        first = false;
        os << "{type_name='" << pair.second->type_name() << "', field_name='" << pair.first << "'}";
    }

    os << "]}";
    return os;
}
} // namespace hotspot::types