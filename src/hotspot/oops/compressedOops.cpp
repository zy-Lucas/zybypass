#include "compressedOops.hpp"

namespace hotspot::oops
{
void CompressedOops::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CompressedOops");

    base_offset = *type->get_field_offset("_narrow_oop._base");
    shift_offset = *type->get_field_offset("_narrow_oop._shift");
}
} // namespace hotspot::oops