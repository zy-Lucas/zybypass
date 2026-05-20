#include "compressedKlassPointers.hpp"

namespace hotspot::oops
{
void CompressedKlassPointers::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CompressedKlassPointers");

    base_offset_ = *type->field_offset("_narrow_klass._base");
    shift_offset_ = *type->field_offset("_narrow_klass._shift");
}
} // namespace hotspot::oops