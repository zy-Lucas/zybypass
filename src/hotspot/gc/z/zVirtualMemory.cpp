#include "zVirtualMemory.hpp"

namespace hotspot::gc::z
{
void ZVirtualMemory::initialize()
{
    utils::FieldResolver r{"ZVirtualMemory"};
    
    r.field_offset("_start", start_offset_);
    r.field_offset("_end", end_offset_);
}
} // namespace hotspot::gc::z