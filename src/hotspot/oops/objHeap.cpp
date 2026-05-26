#include "objHeap.hpp"
#include "oop.hpp"

namespace hotspot::oops
{
oops::Oop ObjHeap::new_oop(debugger::OopHandle handle) noexcept
{
    if (!handle)
        return {Oop::Kind::non_type, 0};
    if (Klass klass{Oop::klass_for_oop_handle(handle)}; klass)
    {
        if (klass.is_typeArray_klass())
            return TypeArray(handle);
        if (klass.is_objArray_klass())
            return ObjArray(handle);
        return Instance(handle);
    }
    return {Oop::Kind::non_type, 0};
}
} // namespace hotspot::oops