#include "zCollectedHeap.hpp"

namespace hotspot::gc::z
{
debugger::OopHandle ZCollectedHeap::oop_load_at(debugger::OopHandle handle, uint64_t offset) const
{
    return oop_load_barrier(runtime::Jvm::read<uint64_t>((uint64_t)handle + offset));
}

debugger::OopHandle ZCollectedHeap::oop_load_in_native(uint64_t addr) const
{
    return oop_load_barrier(runtime::Jvm::read<uint64_t>(addr));
}

void ZCollectedHeap::initialize()
{
    utils::FieldResolver r{"ZCollectedHeap"};

    r.field_offset("_heap", heap_offset_);
}
} // namespace hotspot::gc::z