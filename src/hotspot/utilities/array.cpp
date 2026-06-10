#include "array.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"

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
    utils::FieldResolver r{"Array<int>"};

    r.field_offset("_length", length_offset_);
}

oops::Method MethodArray::at(uint32_t index) const noexcept { return address_at(index); }

void MethodArray::initialize()
{
    utils::FieldResolver r{"Array<Method*>"};

    r.field_offset("_data", data_offset_);

    elem_type_ = utils::FieldResolver{"Method*"}.type();
}

oops::Klass KlassArray::at(uint32_t index) const noexcept { return address_at(index); }

void KlassArray::initialize()
{
    utils::FieldResolver r{"Array<Klass*>"};

    r.field_offset("_data", data_offset_);

    elem_type_ = utils::FieldResolver{"Klass*"}.type();
}

template <typename T> void IntegerArray<T>::initialize()
{
    utils::FieldResolver r{ArrayTypeTraits<T>::array_name};

    r.field_offset("_data", data_offset_);

    elem_type_ = utils::FieldResolver{ArrayTypeTraits<T>::elem_name}.type();
}

template class IntegerArray<uint8_t>;
template class IntegerArray<uint16_t>;
template class IntegerArray<int32_t>;
} // namespace hotspot::utilities