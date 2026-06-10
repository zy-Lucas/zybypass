#include "objArrayKlass.hpp"

namespace hotspot::oops
{
void ObjArrayKlass::initialize()
{
    utils::FieldResolver r{"ObjArrayKlass"};

    r.field_offset("_element_klass", element_Klass_offset_);
    r.field_offset("_bottom_klass", bottom_klass_offset_);
}
} // namespace hotspot::oops