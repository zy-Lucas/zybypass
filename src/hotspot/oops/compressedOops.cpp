#include "compressedOops.hpp"

namespace hotspot::oops
{
void CompressedOops::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CompressedOops");

    base_offset_ = *type->field_offset("_narrow_oop._base");
    shift_offset_ = *type->field_offset("_narrow_oop._shift");
}
} // namespace hotspot::oops