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
    utils::FieldResolver r{"ConstantPool"};

    r.field_offset("_tags", tags_offset_);
    r.field_offset("_cache", cache_offset_);
    r.field_offset("_operands", operands_offset_);
    r.field_offset("_pool_holder", pool_holder_offset_);
    r.field_offset("_resolved_klasses", resolved_klasses_offset_);
    r.field_offset("_major_version", major_version_offset_);
    r.field_offset("_minor_version", minor_version_offset_);
    r.field_offset("_generic_signature_index", generic_signature_index_offset_);
    r.field_offset("_source_file_name_index", source_file_name_index_offset_);
    r.field_offset("_length", length_offset_);

    r.type_size(header_size_);
    element_size_ = runtime::Jvm::oop_size();

    utils::constants::int_const("ConstantPool::_indy_bsm_offset", indy_bsm_offset_);
    utils::constants::int_const("ConstantPool::_indy_argc_offset", indy_argc_offset_);
    utils::constants::int_const("ConstantPool::_indy_argv_offset", indy_argv_offset_);
}
} // namespace hotspot::oops