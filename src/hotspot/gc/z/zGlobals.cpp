#include "zGlobals.hpp"

namespace hotspot::gc::z
{
void ZGlobals::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZGlobalsForVMStructs");

    instance_offset = *type->get_field_offset("_instance_p");
}
} // namespace hotspot::gc::z