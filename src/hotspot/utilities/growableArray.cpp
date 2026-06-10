#include "growableArray.hpp"

namespace hotspot::utilities
{
void GenericGrowableArray::initialize()
{
    utils::FieldResolver r{"GrowableArrayBase"};

    r.field_offset("_len", len_offset_);
    r.field_offset("_max", max_offset_);

    utils::FieldResolver r_arr{"GrowableArray<int>"};

    r_arr.field_offset("_data", data_offset_);
}
} // namespace hotspot::utilities