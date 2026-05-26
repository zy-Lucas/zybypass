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

void ZCollectedHeap::oop_store_at(debugger::OopHandle obj, uint64_t offset, debugger::OopHandle value) const noexcept {}

void ZCollectedHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ZCollectedHeap");

    heap_offset_ = *type->field_offset("_heap");
}
} // namespace hotspot::gc::z