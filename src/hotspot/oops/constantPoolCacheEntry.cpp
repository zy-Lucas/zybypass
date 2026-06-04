#include "constantPoolCacheEntry.hpp"

namespace hotspot::oops
{
ConstantPoolCacheEntry::ConstantPoolCacheEntry(ConstantPoolCache cp, uint32_t index) noexcept
    : cp_(cp), offset_(ConstantPoolCache::base_offset() + index * size_)
{
}

std::pair<std::string_view, runtime::JvmObjectPtr> ConstantPoolCacheEntry::f1() const
{
    return MetaData::instantiate_wrapper_for(runtime::Jvm::read<uint64_t>(cp_.address() + offset_ + f1_offset_));
}

void ConstantPoolCacheEntry::adjust_method_entry(Method old_method, Method new_method)
{
    uint64_t new_addr = new_method.address();
    if (is_vfinal())
    {
        if (f2() == old_method)
            set_f2(new_addr);
        return;
    }
    if (auto [type, ptr]{f1()}; type == "Method")
    {
        if (*ptr == old_method)
            set_f1(new_addr);
        return;
    }
    if (f2() == old_method)
        set_f2(new_addr);
}

Method ConstantPoolCacheEntry::interesting_method_entry() const
{
    if (!is_method_entry())
        return 0;
    if (is_vfinal())
        return f2();
    auto [type, ptr]{f1()};
    if (!ptr)
        return 0;
    if (type == "Method")
        return Method{ptr->address()};
    return f2();
}

void ConstantPoolCacheEntry::initialize()
{
    utils::FieldResolver r{"ConstantPoolCacheEntry"};

    r.field_offset("_indices", indices_offset_);
    r.field_offset("_f1", f1_offset_);
    r.field_offset("_f2", f2_offset_);
    r.field_offset("_flags", flags_offset_);

    r.type_size(size_);

    utils::constants::int_const("ConstantPoolCacheEntry::is_volatile_shift", is_volatile_shift_);
    utils::constants::int_const("ConstantPoolCacheEntry::is_final_shift", is_final_shift_);
    utils::constants::int_const("ConstantPoolCacheEntry::is_forced_virtual_shift", is_forced_virtual_shift_);
    utils::constants::int_const("ConstantPoolCacheEntry::is_vfinal_shift", is_vfinal_shift_);
    utils::constants::int_const("ConstantPoolCacheEntry::is_field_entry_shift", is_field_entry_shift_);
    utils::constants::int_const("ConstantPoolCacheEntry::tos_state_shift", tos_state_shift_);
}
} // namespace hotspot::oops