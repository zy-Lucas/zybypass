#pragma once

#include "field.hpp"
#include <unordered_map>
#include <optional>

namespace hotspot::types
{
class Type
{
  public:
    Type(std::string_view name, Type *super_class, uint64_t size, bool is_oop_type, bool is_int, bool is_unsigned,
         bool is_pointer = false, Type *target_type = nullptr) noexcept;

    Type(const Type &other) = delete;
    Type &operator=(const Type &other) = delete;

    Type(Type &&other) = delete;
    Type &operator=(Type &&other) = delete;

    ~Type() = default;

    std::string_view name() const noexcept { return name_; }

    const Type *super_class() const noexcept { return super_class_; }
    void set_super_class(Type *new_super_class) { super_class_ = new_super_class; }

    uint64_t size() const noexcept { return size_; }
    void set_size(uint64_t new_size) noexcept { size_ = new_size; }

    bool is_oop_type() const noexcept { return is_oop_type_; }
    void set_is_oop_type(bool new_is_oop_type) noexcept { is_oop_type_ = new_is_oop_type; }

    bool is_int() const noexcept { return is_int_; }
    void set_is_int(bool new_is_int) noexcept { is_int_ = new_is_int; }

    bool is_unsigned() const noexcept { return is_unsigned_; }
    void set_is_unsigned(bool new_is_unsigned) noexcept { is_unsigned_ = new_is_unsigned; }

    bool is_pointer() const noexcept { return is_pointer_; }
    void set_is_pointer(bool new_is_pointer, types::Type *new_target_type = nullptr) noexcept;

    const Field *lookup_field(std::string_view field_name) const noexcept; // 只提供临访问Field的函数

    std::optional<uint64_t> field_offset(std::string_view field_name) const noexcept;

    bool add_field(std::unique_ptr<Field> field); // unique_ptr明确所有权

    bool remove_field(std::string_view field_name) { return name_to_field_.erase(field_name) > 0; }

    auto begin() const noexcept { return name_to_field_.begin(); }
    auto end() const noexcept { return name_to_field_.end(); }

  private:
    friend std::ostream &operator<<(std::ostream &os, const Type &type);

    std::string_view name_;
    Type *super_class_;
    uint64_t size_;
    bool is_oop_type_;
    bool is_int_;
    bool is_unsigned_;
    bool is_pointer_;
    Type *target_type_;
    std::unordered_map<std::string_view, std::unique_ptr<Field>> name_to_field_;
};
} // namespace hotspot::types