#include "field.hpp"
#include "../runtime/basicType.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
FieldType::ArrayInfo FieldType::get_array_info() const noexcept
{
    uint32_t index = 1;
    int32_t dim = 1;
    skip_optional_size(signature_, index);
    while (signature_.byte_at(index) == '[')
    {
        index++;
        dim++;
        skip_optional_size(signature_, index);
    }
    return {dim, runtime::BasicType::char_to_type(signature_.byte_at(index))};
}

void FieldType::skip_optional_size(Symbol sig, uint32_t &index) noexcept
{
    uint8_t c = sig.byte_at(index);
    while (c >= '0' && c <= '9')
    {
        ++index;
        c = sig.byte_at(index);
    }
}

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
} // namespace hotspot::oops