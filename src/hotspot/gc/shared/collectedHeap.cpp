#include "collectedHeap.hpp"
#include "oops/oop.hpp"

namespace hotspot::gc::shared
{
debugger::OopHandle CollectedHeap::oop_load_at(debugger::OopHandle handle, uint64_t offset) const
{
    return handle.oop_handle_at(offset);
}

debugger::OopHandle CollectedHeap::oop_load_in_native(uint64_t addr) const
{
    if (!addr)
        return 0;
    return runtime::Jvm::read<uint64_t>(addr);
}

void CollectedHeap::oop_store_at(debugger::OopHandle obj, uint64_t offset, debugger::OopHandle value) const noexcept
{
    if (!obj)
        return;
    runtime::Jvm::write<uint64_t>(obj.address() + offset, value.address());
}

uint64_t CollectedHeap::tlab_alloc_reserve() noexcept
{
    if (uint64_t size = oops::Oop::header_size(); size > runtime::Jvm::min_obj_alignment_in_bytes())
        return oops::Oop::align_object_size(size);
    return 0;
}

void CollectedHeap::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CollectedHeap");

    reserved_offset_ = *type->field_offset("_reserved");
}
} // namespace hotspot::gc::shared