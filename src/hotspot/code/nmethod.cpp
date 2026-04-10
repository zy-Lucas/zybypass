#include "nmethod.hpp"

namespace hotspot::code
{
nmethod::nmethod(uint64_t addr) : CompiledMethod(addr) { STATIC_INIT_GUARD; }

uint32_t nmethod::total_size() const noexcept
{
    return constants_size() + insts_size() + stub_size() + scopes_data_size() + scopes_pcs_size() +
           dependencies_size() + handler_table_size() + nul_chk_table_size();
}

uint64_t nmethod::get_metadata_at(uint32_t index) const noexcept
{
    if (!index)
        return 0;
    return read_field<uint64_t>((index - 1) * (*runtime::Jvm::get_oop_size()), metadata_begin());
}

oops::Method nmethod::get_method(uint32_t index) const
{
    auto [name, base] = oops::MetaData::instantiate_wrapper_for(get_metadata_at(index));
    if (name != "Method")
        throw std::bad_cast();
    return std::bit_cast<oops::Method>(base);
}

void nmethod::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("nmethod");

    entry_bci_offset = *type->get_field_offset("_entry_bci");
    osr_link_offset = *type->get_field_offset("_osr_link");
    state_offset = *type->get_field_offset("_state");
    exception_offset_offset = *type->get_field_offset("_exception_offset");
    orig_pc_offset_offset = *type->get_field_offset("_orig_pc_offset");
    stub_offset_offset = *type->get_field_offset("_stub_offset");
    oops_offset_offset = *type->get_field_offset("_oops_offset");
    metadata_offset_offset = *type->get_field_offset("_metadata_offset");
    scopes_pcs_offset_offset = *type->get_field_offset("_scopes_pcs_offset");
    dependencies_offset_offset = *type->get_field_offset("_dependencies_offset");
    handler_table_offset_offset = *type->get_field_offset("_handler_table_offset");
    nul_chk_table_offset_offset = *type->get_field_offset("_nul_chk_table_offset");
    nmethod_end_offset_offset = *type->get_field_offset("_nmethod_end_offset");
    entry_point_offset = *type->get_field_offset("_entry_point");
    verified_entry_point_offset = *type->get_field_offset("_verified_entry_point");
    osr_entry_point_offset = *type->get_field_offset("_osr_entry_point");
    lock_count_offset = *type->get_field_offset("_lock_count");
    stack_traversal_mark_offset = *type->get_field_offset("_stack_traversal_mark");
    comp_level_offset = *type->get_field_offset("_comp_level");
}
} // namespace hotspot::code