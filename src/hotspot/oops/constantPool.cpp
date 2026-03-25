#include "constantPool.hpp"

namespace hotspot::oops
{
ConstantPool::ConstantPool(uint64_t addr) : runtime::JvmObject<ConstantPool>(addr) {};

void ConstantPool::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstantPool");

    tags_offset = *type->get_field_offset("_tags");
    operands_offset = *type->get_field_offset("_operands");
    cache_offset = *type->get_field_offset("_cache");
    pool_holder_offset = *type->get_field_offset("_pool_holder");
    length_offset = *type->get_field_offset("_length");
    resolved_klasses_offset = *type->get_field_offset("_resolved_klasses");
    major_version_offset = *type->get_field_offset("_major_version");
    minor_version_offset = *type->get_field_offset("_minor_version");
    source_file_name_index_offset = *type->get_field_offset("_source_file_name_index");
    generic_signature_index_offset = *type->get_field_offset("_generic_signature_index");

    header_size = type->get_size();
    element_size = runtime::Jvm::get_oop_size();

    INDY_BSM_OFFSET = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_bsm_offset");
    INDY_ARGC_OFFSET = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argc_offset");
    INDY_ARGV_OFFSET = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argv_offset");
}
} // namespace hotspot::oops