#include "constantPoolCache.hpp"
#include "constantPoolCacheEntry.hpp"

namespace hotspot::oops
{
ConstantPool ConstantPoolCache::constants() const noexcept { return read_field<uint64_t>(constants_offset_); }

ConstantPoolCacheEntry ConstantPoolCache::entry_at(uint32_t index) const noexcept { return {*this, index}; }

void ConstantPoolCache::adjust_method_entry(Method old_method, Method new_method)
{
    int32_t len = length();
    for (uint32_t index = 0; index < len; index++)
    {
        ConstantPoolCacheEntry entry{entry_at(index)};
        if (Method m{entry.interesting_method_entry()}; m == old_method)
            entry.adjust_method_entry(old_method, new_method);
    }
}

void ConstantPoolCache::initialize()
{
    utils::FieldResolver r{"ConstantPoolCache"};

    r.field_offset("_length", length_offset_);
    r.field_offset("_constant_pool", constants_offset_);
    r.field_offset("_resolved_references", resolved_references_offset_);
    r.field_offset("_reference_map", reference_map_offset_);

    r.type_size(base_offset_);

    utils::FieldResolver{"ConstantPoolCacheEntry"}.type_size(element_size_);
}
} // namespace hotspot::oops