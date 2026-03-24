#include "constantPool.hpp"

namespace hotspot::oops
{
ConstantPool::ConstantPool(uint64_t addr) : runtime::JvmObject<ConstantPool>(addr) {};

void ConstantPool::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstantPool");

    tags_offset = type->get_field_offset("_tags").value_or(0);
    operands_offset = type->get_field_offset("_operands").value_or(0);
    cache_offset = type->get_field_offset("_cache").value_or(0);
    pool_holder_offset = type->get_field_offset("_pool_holder").value_or(0);
    length_offset = type->get_field_offset("_length").value_or(0);
    resolved_klasses_offset = type->get_field_offset("_resolved_klasses").value_or(0);
    major_version_offset = type->get_field_offset("_major_version").value_or(0);
    minor_version_offset = type->get_field_offset("_minor_version").value_or(0);
    source_file_name_index_offset = type->get_field_offset("_source_file_name_index").value_or(0);
    generic_signature_index_offset = type->get_field_offset("_generic_signature_index").value_or(0);

    header_size = type->get_size();
    element_size = runtime::Jvm::get_oop_size().value_or(0);

    INDY_BSM_OFFSET = runtime::Jvm::lookup_int_constant("ConstantPool::_indy_bsm_offset").value_or(0);
    INDY_ARGC_OFFSET = runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argc_offset").value_or(0);
    INDY_ARGV_OFFSET = runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argv_offset").value_or(0);
}
} // namespace hotspot::oops