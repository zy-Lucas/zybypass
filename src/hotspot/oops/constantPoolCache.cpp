#include "constantPoolCache.hpp"
#include "constantPool.hpp"

namespace hotspot::oops
{
ConstantPool ConstantPoolCache::constants() const noexcept { return read_field<uint64_t>(constants_offset_); }

void ConstantPoolCache::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstantPoolCache");

    length_offset_ = *type->field_offset("_length");
    constants_offset_ = *type->field_offset("_constant_pool");
    resolved_references_offset_ = *type->field_offset("_resolved_references");
    reference_map_offset_ = *type->field_offset("_reference_map");

    base_offset_ = type->size();

    element_size_ = runtime::Jvm::lookup_type("ConstantPoolCacheEntry")->size();
}
} // namespace hotspot::oops