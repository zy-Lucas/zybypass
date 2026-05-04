#include "constantPool.hpp"
#include "instanceKlass.hpp"

namespace hotspot::oops
{
InstanceKlass ConstantPool::get_pool_holder() const noexcept { return read_field<uint64_t>(pool_holder_offset); }

Symbol ConstantPool::get_generic_signature() const noexcept
{
    return get_symbol_at(read_field<uint16_t>(generic_signature_index_offset));
}

Symbol ConstantPool::get_source_file_name() const noexcept
{
    return get_symbol_at(read_field<uint16_t>(source_file_name_index_offset));
}

utilities::KlassArray ConstantPool::get_resolved_klasses() const noexcept
{
    return read_field<uint64_t>(resolved_klasses_offset);
}

CPKlassSlot ConstantPool::get_klass_slot_at(uint32_t index) const noexcept
{
    int value = get_int_at(index);
    return {(uint16_t)(value >> 16), (uint16_t)value};
}

int64_t ConstantPool::get_long_at(uint32_t index) const noexcept
{
    return runtime::Jvm::build_long_from_intsPD(read_field<int32_t>(index_offset(index)),
                                                read_field<int32_t>(index_offset(index + 1)));
}

void ConstantPool::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstantPool");

    tags_offset = *type->get_field_offset("_tags");
    cache_offset = *type->get_field_offset("_cache");
    operands_offset = *type->get_field_offset("_operands");
    pool_holder_offset = *type->get_field_offset("_pool_holder");
    resolved_klasses_offset = *type->get_field_offset("_resolved_klasses");
    major_version_offset = *type->get_field_offset("_major_version");
    minor_version_offset = *type->get_field_offset("_minor_version");
    generic_signature_index_offset = *type->get_field_offset("_generic_signature_index");
    source_file_name_index_offset = *type->get_field_offset("_source_file_name_index");
    length_offset = *type->get_field_offset("_length");

    header_size = type->get_size();
    element_size = runtime::Jvm::get_oop_size();

    INDY_BSM_OFFSET = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_bsm_offset");
    INDY_ARGC_OFFSET = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argc_offset");
    INDY_ARGV_OFFSET = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argv_offset");
}
} // namespace hotspot::oops