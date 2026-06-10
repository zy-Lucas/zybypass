#include "method.hpp"
#include "code/nmethod.hpp"
#include "instanceKlass.hpp"

namespace hotspot::oops
{
ConstMethod Method::constMethod() const noexcept { return read_field<uint64_t>(constMethod_offset_); }

code::nmethod Method::native_method() const noexcept { return read_field<uint64_t>(code_offset_); }

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

uint64_t Method::find_jmethod_id_or_null() const noexcept
{
    return constants().pool_holder().jmethod_id_or_null(*this);
}

void Method::change_method_associated_with_jmethod_id(uint64_t old_jmid_ptr, Method new_method) noexcept
{
    runtime::Jvm::write(old_jmid_ptr, new_method.address());
}

void Method::initialize()
{
    utils::FieldResolver r{"Method"};

    r.field_offset("_constMethod", constMethod_offset_);
    r.field_offset("_method_data", method_data_offset_);
    r.field_offset("_method_counters", method_counters_offset_);
    r.field_offset("_access_flags", access_flags_offset_);
    r.field_offset("_vtable_index", vtable_index_offset_);
    r.field_offset("_from_compiled_entry", from_compiled_entry_offset_);
    r.field_offset("_code", code_offset_);
    r.field_offset("_from_interpreted_entry", from_interpreter_entry_offset_);

    type_ = r.type();
}
} // namespace hotspot::oops