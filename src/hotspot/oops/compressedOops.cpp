#include "compressedOops.hpp"

namespace hotspot::oops
{
void CompressedOops::initialize()
{
    utils::FieldResolver r{"CompressedOops"};

    r.field_offset("_narrow_oop._base", base_offset_);
    r.field_offset("_narrow_oop._shift", shift_offset_);
}
} // namespace hotspot::oops