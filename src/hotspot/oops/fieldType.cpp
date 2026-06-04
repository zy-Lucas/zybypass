#include "fieldType.hpp"
#include "runtime/basicType.hpp"

namespace hotspot::oops
{
FieldType::ArrayInfo FieldType::array_info() const noexcept
{
    uint32_t index = 1;
    int32_t dim = 1;
    skip_optional_size(signature_, index);
    while (signature_.byte_at(index) == '[')
    {
        ++index;
        ++dim;
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
} // namespace hotspot::oops