#pragma once

#include "runtime/jvm.hpp"

namespace hotspot::utils
{
class FieldResolver
{
  public:
    explicit FieldResolver(std::string_view vm_class_name) noexcept : type_(runtime::Jvm::lookup_type(vm_class_name)) {}

    void field_offset(std::string_view vm_name, uint64_t &dest) const noexcept { dest = *type_->field_offset(vm_name); }
    uint64_t field_offset(std::string_view vm_name) const noexcept { return *type_->field_offset(vm_name); }

    void type_size(uint64_t &dest) const noexcept { dest = type_->size(); }
    uint64_t type_size() const noexcept { return type_->size(); }

    types::Type *type() const noexcept { return type_; }

  private:
    types::Type *type_;
};

namespace constants
{
void int_const(std::string_view name, int32_t &dest) noexcept;
void long_const(std::string_view name, int64_t &dest) noexcept;
} // namespace constants
} // namespace hotspot::utils