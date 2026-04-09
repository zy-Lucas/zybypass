#include "method.hpp"
#include <cstdint>

namespace hotspot::oops
{
Method::Method(uint64_t addr) : MetaData(addr) { STATIC_INIT_GUARD; }

Symbol Method::get_generic_signature() const noexcept
{
    uint64_t index = get_generic_signature_index();
    return index ? get_constants().get_symbol_at(index) : 0;
}

bool Method::is_package_private() const noexcept
{
    runtime::AccessFlags af = get_access_flags_obj();
    return (!af.is_public() && !af.is_private() && !af.is_protected());
}

void Method::initialize()
{
    type = runtime::Jvm::lookup_type("Method");

    constMethod_offset = *type->get_field_offset("_constMethod");
    method_data_offset = *type->get_field_offset("_method_data");
    method_counters_offset = *type->get_field_offset("_method_counters");
    access_flags_offset = *type->get_field_offset("_access_flags");
    vtable_index_offset = *type->get_field_offset("_vtable_index");
    code_offset = *type->get_field_offset("_code");
}
} // namespace hotspot::oops