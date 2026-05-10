#include "objArrayKlass.hpp"

namespace hotspot::oops
{
void ObjArrayKlass::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ObjArrayKlass");

    element_Klass_offset = *type->get_field_offset("_element_klass");
    bottom_klass_offset = *type->get_field_offset("_bottom_klass");
}
} // namespace hotspot::oops