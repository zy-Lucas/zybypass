#include "arrayKlass.hpp"

namespace hotspot::oops
{
void ArrayKlass::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ArrayKlass");

    dimension_offset = *type->get_field_offset("_dimension");
    higher_dimension_offset = *type->get_field_offset("_higher_dimension");
    lower_dimension_offset = *type->get_field_offset("_lower_dimension");
}
}