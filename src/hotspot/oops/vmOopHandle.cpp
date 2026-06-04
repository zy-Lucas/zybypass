#include "vmOopHandle.hpp"
#include "memory/universe.hpp"
#include "objHeap.hpp"
#include "oop.hpp"

namespace hotspot::oops
{
Instance VMOopHandle::resolve() const
{
    if (uint64_t handle = obj(); handle)
        return ObjHeap::new_oop(memory::Universe::heap()->oop_load_in_native(handle));
    return {0};
}

void VMOopHandle::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("OopHandle");

    obj_offset = *type->field_offset("_obj");
}
} // namespace hotspot::oops