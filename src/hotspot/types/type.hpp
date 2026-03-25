#pragma once

#include "field.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

namespace hotspot::types
{
class Type
{
  public:
    Type(std::string_view name, Type *super_class, size_t size, bool is_oop_type, bool is_int, bool is_unsigned,
         bool is_pointer = false, Type *target_type = nullptr);

    Type(const Type &other) = delete;
    Type &operator=(const Type &other) = delete;

    Type(Type &&other) = delete;
    Type &operator=(Type &&other) = delete;

    ~Type() = default;

    std::string_view get_name() const noexcept { return name; }
    const Type *get_super_class() const noexcept { return super_class; }
    size_t get_size() const noexcept { return size; }
    bool get_is_oop_type() const noexcept { return is_oop_type; }
    bool get_is_int() const noexcept { return is_int; }
    bool get_is_unsigned() const noexcept { return is_unsigned; }
    bool get_is_pointer() const noexcept { return is_pointer; }

    const Field *get_field(std::string_view field_name) const noexcept; // 只提供临访问Field的函数

    std::optional<uint64_t> get_field_offset(std::string_view field_name) const noexcept;

    void set_super_class(Type *new_super_class) { super_class = new_super_class; }
    void set_size(size_t new_size) noexcept { size = new_size; }
    void set_is_oop_type(bool new_is_oop_type) noexcept { is_oop_type = new_is_oop_type; }
    void set_is_int(bool new_is_int) noexcept { is_int = new_is_int; }
    void set_is_unsigned(bool new_is_unsigned) noexcept { is_unsigned = new_is_unsigned; }
    void set_is_pointer(bool new_is_pointer, types::Type *new_target_type = nullptr) noexcept
    {
        is_pointer = new_is_pointer;
        target_type = new_target_type;
    }

    bool add_field(std::unique_ptr<Field> field); // unique_ptr明确所有权

    bool remove_field(std::string_view field_name) { return name_to_field.erase(field_name) > 0; }

    auto begin() const noexcept { return name_to_field.begin(); }
    auto end() const noexcept { return name_to_field.end(); }

  private:
    friend std::ostream &operator<<(std::ostream &os, const Type &type);

    std::string_view name;
    Type *super_class;
    size_t size;
    bool is_oop_type;
    bool is_int;
    bool is_unsigned;
    bool is_pointer;
    Type *target_type;
    std::unordered_map<std::string_view, std::unique_ptr<Field>> name_to_field;
};
} // namespace hotspot::types