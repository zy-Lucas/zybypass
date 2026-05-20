#include "constantPool.hpp"
#include "instanceKlass.hpp"

namespace hotspot::oops
{
InstanceKlass ConstantPool::pool_holder() const noexcept { return read_field<uint64_t>(pool_holder_offset_); }

Symbol ConstantPool::generic_signature() const noexcept
{
    return symbol_at(read_field<uint16_t>(generic_signature_index_offset_));
}

Symbol ConstantPool::source_file_name() const noexcept
{
    return symbol_at(read_field<uint16_t>(source_file_name_index_offset_));
}

utilities::KlassArray ConstantPool::resolved_klasses() const noexcept
{
    return read_field<uint64_t>(resolved_klasses_offset_);
}

CPKlassSlot ConstantPool::get_klass_slot_at(uint32_t index) const noexcept
{
    int32_t value = int_at(index);
    return {(uint16_t)(value >> 16), (uint16_t)value};
}

int64_t ConstantPool::long_at(uint32_t index) const noexcept
{
    return runtime::Jvm::build_long_from_intsPD(read_field<int32_t>(index_offset(index)),
                                                read_field<int32_t>(index_offset(index + 1)));
}

void ConstantPool::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ConstantPool");

    tags_offset_ = *type->field_offset("_tags");
    cache_offset_ = *type->field_offset("_cache");
    operands_offset_ = *type->field_offset("_operands");
    pool_holder_offset_ = *type->field_offset("_pool_holder");
    resolved_klasses_offset_ = *type->field_offset("_resolved_klasses");
    major_version_offset_ = *type->field_offset("_major_version");
    minor_version_offset_ = *type->field_offset("_minor_version");
    generic_signature_index_offset_ = *type->field_offset("_generic_signature_index");
    source_file_name_index_offset_ = *type->field_offset("_source_file_name_index");
    length_offset_ = *type->field_offset("_length");

    header_size_ = type->size();
    element_size_ = runtime::Jvm::oop_size();

    indy_bsm_offset_ = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_bsm_offset");
    indy_argc_offset_ = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argc_offset");
    indy_argv_offset_ = *runtime::Jvm::lookup_int_constant("ConstantPool::_indy_argv_offset");
}
} // namespace hotspot::oops