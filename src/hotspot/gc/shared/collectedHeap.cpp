#include "collectedHeap.hpp"

namespace hotspot::gc::shared
{
debugger::OopHandle CollectedHeap::oop_load_at(debugger::OopHandle handle, uint64_t offset) const noexcept
{
    return handle.get_oop_handle_at(offset);
}

debugger::OopHandle CollectedHeap::oop_load_in_native(uint64_t addr) const noexcept
{
    return runtime::Jvm::read<uint64_t>(addr);
}

void CollectedHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CollectedHeap");

    reserved_offset = *type->get_field_offset("_reserved");
}
} // namespace hotspot::gc::shared