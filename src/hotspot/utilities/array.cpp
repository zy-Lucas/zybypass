#include "array.hpp"
#include "../oops/klass.hpp"
#include "../oops/method.hpp"

namespace hotspot::utilities
{
uint64_t GenericArray::get_size() const noexcept
{
    static int32_t bpw = runtime::Jvm::get_bytes_per_word();
    return runtime::Jvm::align_up(byte_sizeof(length()), bpw) / bpw;
}

int64_t GenericArray::get_integer_at(uint32_t index) const noexcept
{
    if (index >= length())
        return 0;
    types::Type *elem_type = get_elem_type();
    if (!elem_type->get_is_int())
        return 0;
    uint64_t elem_size = elem_type->get_size();
    return runtime::Jvm::read<int64_t>(address() + data_offset + index * elem_size, elem_size);
}

uint64_t GenericArray::get_address_at(uint32_t index) const noexcept
{
    if (index >= length())
        return 0;
    types::Type *elem_type = get_elem_type();
    if (elem_type->get_is_int())
        return 0;
    return read_field<uint64_t>(data_offset + index * elem_type->get_size());
}

void GenericArray::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Array<int>");

    length_offset = *type->get_field_offset("_length");
}

oops::Method MethodArray::at(uint32_t i) const noexcept { return get_address_at(i); }

void MethodArray::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Array<Method*>");
    elem_type = runtime::Jvm::lookup_type("Method*");

    data_offset = *type->get_field_offset("_data");
}

oops::Klass KlassArray::at(uint32_t i) const noexcept { return get_address_at(i); }

void KlassArray::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Array<Klass*>");
    elem_type = runtime::Jvm::lookup_type("Klass*");

    data_offset = *type->get_field_offset("_data");
}

template <typename T> void IntegerArray<T>::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type(ArrayTypeTraits<T>::array_name);
    elem_type = runtime::Jvm::lookup_type(ArrayTypeTraits<T>::elem_name);

    data_offset = *type->get_field_offset("_data");
}

template class IntegerArray<uint8_t>;
template class IntegerArray<uint16_t>;
template class IntegerArray<int32_t>;
} // namespace hotspot::utilities