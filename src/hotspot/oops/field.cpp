#include "field.hpp"
#include "../runtime/basicType.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
FieldType::ArrayInfo FieldType::get_array_info() const noexcept
{
    uint32_t index = 1;
    int32_t dim = 1;
    skip_optional_size(signature, index);
    while (signature.get_byte_at(index) == '[')
    {
        index++;
        dim++;
        skip_optional_size(signature, index);
    }
    return {dim, runtime::BasicType::char_to_type(signature.get_byte_at(index))};
}

void FieldType::skip_optional_size(Symbol sig, uint32_t &index) noexcept
{
    uint8_t c = sig.get_byte_at(index);
    while (c >= '0' && c <= '9')
    {
        ++index;
        c = sig.get_byte_at(index);
    }
}

Field::Field(uint64_t offset, FieldIdentifier id, bool is_vm_field)
    : offset(offset), id(std::move(id)), vm_field(is_vm_field)
{
}

Field::Field(InstanceKlass holder, uint32_t field_index)
    : holder(holder), field_index(field_index), offset(holder.get_field_offset(field_index)),
      generic_signature(holder.get_field_generic_signature(field_index)), name(holder.get_field_name(field_index)),
      id(NamedFieldIdentifier(name.as_view())), signature(holder.get_field_signature(field_index)),
      field_type(signature), access_flags(holder.get_field_access_flags(field_index))
{
}

bool Field::operator==(const Field &other) const
{
    if (!holder || !other.holder)
        return false;
    return holder == other.holder && id == other.id;
}

size_t Field::hash_code() const noexcept
{
    if (!holder)
        return 0;
    return holder.hash_code() ^ ::hotspot::oops::hash_code(id);
}
} // namespace hotspot::oops