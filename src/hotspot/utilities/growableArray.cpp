#include "growableArray.hpp"

namespace hotspot::utilities
{
void GenericGrowableArray::initialize()
{
    types::Type *type_base = runtime::Jvm::lookup_type("GrowableArrayBase");
    types::Type *type_array = runtime::Jvm::lookup_type("GrowableArray<int>");

    len_offset = *type_base->get_field_offset("_len");
    max_offset = *type_base->get_field_offset("_max");

    data_offset = *type_array->get_field_offset("_data");
}
} // namespace hotspot::utilities