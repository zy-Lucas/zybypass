#pragma once

#include <iostream>

namespace hotspot::types
{
class Type;

class Field
{
  public:
    Field(std::string_view type_name, std::string_view field_name, Type *field_type, bool is_static, uint64_t offset);

    Field(const Field &) = delete;
    Field &operator=(const Field &) = delete;
    Field(Field &&) = delete;
    Field &operator=(Field &&) = delete;

    ~Field() = default;

    std::string_view get_type_name() const noexcept { return type_name; }
    std::string_view get_field_name() const noexcept { return field_name; }
    bool get_is_static() const noexcept { return is_static; }
    uint64_t get_offset() const noexcept { return offset; }
    const Type *get_field_type() const noexcept { return field_type; }

    void set_type_name(std::string_view new_type_name) { type_name = new_type_name; }
    void set_field_name(std::string_view new_field_name) { field_name = new_field_name; }
    void set_is_static(bool new_is_static) noexcept { is_static = new_is_static; }
    void set_offset(uint64_t new_offset) noexcept { offset = new_offset; }
    void set_field_type(Type *new_field_type) noexcept { field_type = new_field_type; }

  private:
    friend std::ostream &operator<<(std::ostream &os, const Field &field);

    std::string_view type_name;
    std::string_view field_name;
    bool is_static;
    uint64_t offset;
    Type *field_type;
};
} // namespace hotspot::types