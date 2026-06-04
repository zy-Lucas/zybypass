#include "field.hpp"
#include "memory/universe.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
Field::Field(uint64_t offset, FieldIdentifier id, bool is_vm_field) noexcept
    : offset_(offset), id_(std::move(id)), vm_field_(is_vm_field)
{
}

Field::Field(InstanceKlass holder, uint32_t field_index) noexcept
    : holder_(holder), field_index_(field_index), offset_(holder.field_offset(field_index)),
      generic_signature_(holder.field_generic_signature(field_index)), name_(holder.field_name(field_index)),
      id_(NamedFieldIdentifier(name_.as_view())), signature_(holder.field_signature(field_index)),
      field_type_(signature_), access_flags_(holder.field_access_flags(field_index))
{
}

bool Field::operator==(const Field &other) const
{
    if (!holder_ || !other.holder_)
        return false;
    return holder_ == other.holder_ && id_ == other.id_;
}

size_t Field::hash_code() const noexcept
{
    if (!holder_)
        return 0;
    return holder_.hash_code() ^ ::hotspot::oops::hash_code(id_);
}

OopField::OopField(uint64_t offset, FieldIdentifier id, bool is_vm_field) noexcept
    : Field(offset, std::move(id), is_vm_field)
{
}

debugger::OopHandle OopField::value_as_oop_handle(const Oop &obj) const
{
    if (runtime::Jvm::is_compressed_oops_enabled())
        return obj.handle().compressed_oop_handle_at(offset());
    return memory::Universe::heap()->oop_load_at(obj.handle(), offset());
}

void OopField::set_value(const Oop &obj, const Oop &value) noexcept
{
    if (runtime::Jvm::is_compressed_oops_enabled())
        obj.handle().write_compressed_oop_handle_at(offset(), value.handle());
    else
        memory::Universe::heap()->oop_store_at(obj.handle(), offset(), value.handle());
}
} // namespace hotspot::oops