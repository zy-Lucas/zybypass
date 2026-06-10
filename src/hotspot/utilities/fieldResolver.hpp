#pragma once

#include "runtime/jvm.hpp"

namespace hotspot::utils
{
class FieldResolver
{
  public:
    explicit FieldResolver(std::string_view class_name) noexcept : type_(runtime::Jvm::lookup_type(class_name)) {}

    void field_offset(std::string_view name, uint64_t &dest) const noexcept { dest = *type_->field_offset(name); }
    void field_offset(std::string_view name, std::optional<uint64_t> &dest) const noexcept;
    uint64_t field_offset(std::string_view name) const noexcept { return *type_->field_offset(name); }

    void type_size(uint64_t &dest) const noexcept { dest = type_->size(); }
    uint64_t type_size() const noexcept { return type_->size(); }

    types::Type *type() const noexcept { return type_; }

  private:
    types::Type *type_;
};

namespace constants
{
template <std::integral T> void int_const(std::string_view name, T &dest) noexcept
{
    dest = static_cast<T>(*runtime::Jvm::lookup_int_constant(name));
}

template <std::integral T> void long_const(std::string_view name, T &dest) noexcept
{
    dest = static_cast<T>(*runtime::Jvm::lookup_long_constant(name));
}
} // namespace constants
} // namespace hotspot::utils