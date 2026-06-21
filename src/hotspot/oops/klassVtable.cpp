#include "klassVtable.hpp"

namespace hotspot::oops
{
itableMethodEntry *itableOffsetEntry::method_entry(Klass k, int32_t offset) noexcept
{
    return (itableMethodEntry *)(k.address() + offset);
}

klassVtable::klassVtable(Klass klass, uint64_t base, int32_t length) noexcept
    : klass_(klass), table_offset_(base - klass.address()), length_(length)
{
}

void klassVtable::replace_method(Method old_method, Method new_method) noexcept
{
    if (!old_method || !new_method || old_method.vtable_index() >= length_)
        return;
    vtable()[old_method.vtable_index()].set_method(new_method);
}

void klassVtable::adjust_default_method(Method old_method, Method new_method) noexcept
{
    if (!old_method || !new_method)
        return;
    for (uint32_t index = 0; index < length_; ++index)
        if (vtableEntry &entry = vtable()[index]; entry.method() == old_method)
            entry.set_method(new_method);
}

klassItable::klassItable(InstanceKlass klass) noexcept
    : klass_(klass), table_offset_(0), size_offset_table_(0), size_method_table_(0)
{
    if (!klass || !klass.itable_length())
        return;
    itableOffsetEntry *offset_entry = (itableOffsetEntry *)klass.start_of_itable();
    if (!offset_entry || !offset_entry->interface_klass())
        return;

    uint64_t klass_addr = klass.address();
    uint64_t itable_start = klass.start_of_itable();
    uint64_t method_entry = klass_addr + offset_entry->offset();

    table_offset_ = itable_start - klass_addr;
    size_offset_table_ = method_entry - itable_start;
    size_method_table_ = klass.end_of_itable() - method_entry;
}

itableOffsetEntry *klassItable::offset_entry(uint32_t index) const noexcept
{
    return &((itableOffsetEntry *)vtable_start())[index];
}

itableMethodEntry *klassItable::method_entry(uint32_t index) const noexcept
{
    return &((itableMethodEntry *)method_start())[index];
}

bool klassItable::replace_method(Method old_method, Method new_method) noexcept
{
    itableMethodEntry *ime = method_entry(0);
    uint32_t size = size_method_table_ / sizeof(itableMethodEntry);
    bool replaced = false;

    for (uint32_t index = 0; index < size; index++, ime++)
    {
        if (ime->method() == old_method)
        {
            ime->set_method(new_method);
            replaced = true;
        }
    }
    return replaced;
}
} // namespace hotspot::oops