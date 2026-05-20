#include "array.hpp"
#include "../oops/klass.hpp"
#include "../oops/method.hpp"

namespace hotspot::utilities
{
uint64_t GenericArray::size() const noexcept
{
    static int32_t bpw = runtime::Jvm::bytes_per_word();
    return runtime::Jvm::align_up(byte_sizeof(length()), bpw) / bpw;
}

int64_t GenericArray::integer_at(uint32_t index) const noexcept
{
    if (index >= length())
        return 0;
    types::Type *type = elem_type();
    if (!type->is_int())
        return 0;
    uint64_t elem_size = type->size();
    return runtime::Jvm::read<int64_t>(address() + data_offset_ + index * elem_size, elem_size);
}

uint64_t GenericArray::address_at(uint32_t index) const noexcept
{
    if (index >= length())
        return 0;
    types::Type *type = elem_type();
    if (type->is_int())
        return 0;
    return read_field<uint64_t>(data_offset_ + index * type->size());
}

void GenericArray::set_address_at(uint32_t index, uint64_t addr) noexcept
{
    if (index >= length())
        return;
    types::Type *type = elem_type();
    if (type->is_int())
        return;
    write_field<uint64_t>(data_offset_ + index * type->size(), addr);
}

void GenericArray::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Array<int>");

    length_offset_ = *type->field_offset("_length");
}

oops::Method MethodArray::at(uint32_t index) const noexcept { return address_at(index); }

void MethodArray::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Array<Method*>");
    elem_type_ = runtime::Jvm::lookup_type("Method*");

    data_offset_ = *type->field_offset("_data");
}

oops::Klass KlassArray::at(uint32_t index) const noexcept { return address_at(index); }

void KlassArray::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Array<Klass*>");
    elem_type_ = runtime::Jvm::lookup_type("Klass*");

    data_offset_ = *type->field_offset("_data");
}

template <typename T> void IntegerArray<T>::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type(ArrayTypeTraits<T>::array_name);
    elem_type_ = runtime::Jvm::lookup_type(ArrayTypeTraits<T>::elem_name);

    data_offset_ = *type->field_offset("_data");
}

template class IntegerArray<uint8_t>;
template class IntegerArray<uint16_t>;
template class IntegerArray<int32_t>;
} // namespace hotspot::utilities