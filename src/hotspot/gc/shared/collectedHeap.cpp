#include "collectedHeap.hpp"

namespace hotspot::gc::shared
{
debugger::OopHandle CollectedHeap::oop_load_at(debugger::OopHandle handle, uint64_t offset) const
{
    return handle.oop_handle_at(offset);
}

debugger::OopHandle CollectedHeap::oop_load_in_native(uint64_t addr) const
{
    return runtime::Jvm::read<uint64_t>(addr);
}

void CollectedHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CollectedHeap");

    reserved_offset_ = *type->field_offset("_reserved");
}
} // namespace hotspot::gc::shared