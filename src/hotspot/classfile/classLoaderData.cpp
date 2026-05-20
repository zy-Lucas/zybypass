#include "classLoaderData.hpp"
#include "../oops/instanceKlass.hpp"

namespace hotspot::classfile
{
oops::Klass ClassLoaderData::find(std::string_view class_name) const noexcept
{
    for (oops::Klass k{klasses()}; k; k = k.next_link())
    {
        if (!k.name().equals(class_name))
            continue;
        if (!k.is_instance_klass() || oops::InstanceKlass{k.address()}.is_loaded())
            return k;
    }
    return 0;
}

void ClassLoaderData::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ClassLoaderData");

    class_loader_offset_ = *type->field_offset("_class_loader");
    has_class_mirror_holder_offset_ = *type->field_offset("_has_class_mirror_holder");
    klasses_offset_ = *type->field_offset("_klasses");
    dictionary_offset_ = *type->field_offset("_dictionary");
    next_offset_ = *type->field_offset("_next");
}
} // namespace hotspot::classfile