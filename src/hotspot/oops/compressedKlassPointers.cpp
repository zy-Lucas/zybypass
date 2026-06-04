#include "compressedKlassPointers.hpp"

namespace hotspot::oops
{
void CompressedKlassPointers::initialize()
{
    utils::FieldResolver r{"CompressedKlassPointers"};

    r.field_offset("_narrow_klass._base", base_offset_);
    r.field_offset("_narrow_klass._shift", shift_offset_);
}
} // namespace hotspot::oops