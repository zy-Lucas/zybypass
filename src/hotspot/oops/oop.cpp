#include "oop.hpp"

namespace hotspot::oops
{
Klass Oop::get_klass() const noexcept
{
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        return get_handle().get_comp_klass_address_at(compressed_klass_offset);
    return runtime::Jvm::read<uint64_t>(get_handle().address() + klass_offset);
}

Klass Oop::get_klass_for_oop_handle(debugger::OopHandle handle)
{
    if (!handle)
        return 0;
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        return handle.get_comp_klass_address_at(compressed_klass_offset);
    return runtime::Jvm::read<uint64_t>(handle.address() + klass_offset);
}

void Oop::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("oopDesc");

    mark_offset = *type->get_field_offset("_mark");
    klass_offset = *type->get_field_offset("_metadata._klass");
    compressed_klass_offset = *type->get_field_offset("_metadata._compressed_klass");

    header_size = type->get_size();
}
} // namespace hotspot::oops