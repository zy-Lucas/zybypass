#include "arrayKlass.hpp"

namespace hotspot::oops
{
bool ArrayKlass::compute_subtype_of(Klass k) const noexcept
{
    if (Symbol name{k.name()};
        name.equals("java/io/Serializable") || name.equals("java/lang/Cloneable") || name.equals("java/lang/Object"))
        return true;
    return false;
}

void ArrayKlass::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ArrayKlass");

    dimension_offset_ = *type->field_offset("_dimension");
    higher_dimension_offset_ = *type->field_offset("_higher_dimension");
    lower_dimension_offset_ = *type->field_offset("_lower_dimension");
}
} // namespace hotspot::oops