#include "method.hpp"
#include "../code/nmethod.hpp"

namespace hotspot::oops
{
ConstMethod Method::constMethod() const noexcept { return read_field<uint64_t>(constMethod_offset_); }

code::nmethod Method::native_method() const noexcept { return read_field<uint64_t>(code_offset_); }

void Method::set_from_interpreter_entry(uint64_t addr) noexcept { write_field(from_interpreter_entry_offset_, addr); }

void Method::set_flag(int32_t flag, bool value) noexcept
{
    value ? set_access_flags(access_flags() | flag) : set_access_flags(access_flags() & ~flag);
}

bool Method::is_package_private() const noexcept
{
    runtime::AccessFlags af = access_flags_obj();
    return !af.is_public() && !af.is_private() && !af.is_protected();
}

void Method::set_is_package_private(bool value) noexcept
{
    if (value)
        set_access_flags(access_flags() &
                         ~(runtime::JVM_ACC_PUBLIC | runtime::JVM_ACC_PRIVATE | runtime::JVM_ACC_PROTECTED));
}

void Method::initialize()
{
    type_ = runtime::Jvm::lookup_type("Method");

    constMethod_offset_ = *type_->field_offset("_constMethod");
    method_data_offset_ = *type_->field_offset("_method_data");
    method_counters_offset_ = *type_->field_offset("_method_counters");
    access_flags_offset_ = *type_->field_offset("_access_flags");
    vtable_index_offset_ = *type_->field_offset("_vtable_index");
    code_offset_ = *type_->field_offset("_code");
    from_interpreter_entry_offset_ = *type_->field_offset("_from_interpreted_entry");
}
} // namespace hotspot::oops