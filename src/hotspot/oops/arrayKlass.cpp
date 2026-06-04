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
    utils::FieldResolver r{"ArrayKlass"};

    r.field_offset("_dimension", dimension_offset_);
    r.field_offset("_higher_dimension", higher_dimension_offset_);
    r.field_offset("_lower_dimension", lower_dimension_offset_);
}
} // namespace hotspot::oops