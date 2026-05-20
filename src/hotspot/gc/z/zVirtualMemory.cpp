#include "zVirtualMemory.hpp"

namespace hotspot::gc::z
{
void ZVirtualMemory::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZVirtualMemory");

    start_offset_ = *type->field_offset("_start");
    end_offset_ = *type->field_offset("_end");
}
} // namespace hotspot::gc::z