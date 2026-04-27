#include "constantPoolCache.hpp"
#include "constantPool.hpp"

namespace hotspot::oops
{
ConstantPool ConstantPoolCache::get_constants() const noexcept { return read_field<uint64_t>(constants_offset); }

void ConstantPoolCache::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstantPoolCache");

    length_offset = *type->get_field_offset("_length");
    constants_offset = *type->get_field_offset("_constant_pool");
    resolved_references_offset = *type->get_field_offset("_resolved_references");
    reference_map_offset = *type->get_field_offset("_reference_map");

    base_offset = type->get_size();

    element_size = runtime::Jvm::lookup_type("ConstantPoolCacheEntry")->get_size();
    int_size = sizeof(int);
}
} // namespace hotspot::oops