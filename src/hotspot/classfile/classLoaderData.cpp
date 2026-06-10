#include "classLoaderData.hpp"
#include "oops/instanceKlass.hpp"

namespace hotspot::classfile
{
ClassLoaderData ClassLoaderData::next() const noexcept
{
    return atomic_load_field<uint64_t>(next_offset_, std::memory_order_relaxed);
}

oops::Klass ClassLoaderData::find(std::string_view class_name) const noexcept
{
    for (oops::Klass k{klasses()}; k; k = k.next_link())
    {
        if (!k.name().equals(class_name))
            continue;
        if (!k.is_instance_klass() || oops::InstanceKlass{k}.is_loaded())
            return k;
    }
    return 0;
}

void ClassLoaderData::initialize()
{
    utils::FieldResolver r{"ClassLoaderData"};

    r.field_offset("_class_loader", class_loader_offset_);
    r.field_offset("_has_class_mirror_holder", has_class_mirror_holder_offset_);
    r.field_offset("_klasses", klasses_offset_);
    r.field_offset("_dictionary", dictionary_offset_);
    r.field_offset("_next", next_offset_);

    metaspace_offset_ = class_loader_offset_ + sizeof(void *);
}
} // namespace hotspot::classfile