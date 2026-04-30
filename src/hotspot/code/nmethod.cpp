#include "nmethod.hpp"
#include "pcDesc.hpp"
#include "scopeDesc.hpp"

namespace hotspot::code
{
uint32_t nmethod::total_size() const noexcept
{
    return constants_size() + insts_size() + stub_size() + scopes_data_size() + scopes_pcs_size() +
           dependencies_size() + handler_table_size() + nul_chk_table_size();
}

uint64_t nmethod::get_metadata_at(uint32_t index) const noexcept
{
    if (!index)
        return 0;
    return runtime::Jvm::read<uint64_t>((index - 1) * (*runtime::Jvm::get_oop_size()) + metadata_begin());
}

oops::Method nmethod::get_method(uint32_t index) const noexcept
{
    // auto [name, base] = oops::MetaData::instantiate_wrapper_for(get_metadata_at(index));
    // if (name != "Method")
    //     throw std::bad_cast();
    return get_metadata_at(index);
}

bool nmethod::is_method_handle_return(uint64_t return_pc) const noexcept
{
    if (PcDesc pd{get_pc_desc_at(return_pc)}; pd)
        return pd.is_method_handle_invoke();
    return false;
}

PcDesc nmethod::get_pc_desc_at(uint64_t pc) const noexcept
{
    if (pc < code_begin())
        return 0;
    for (uint64_t p = scopes_pcs_begin(); p < scopes_pcs_end(); p += PcDesc::pc_desc_size)
        if (PcDesc pcDesc{p}; pcDesc.get_real_pc(*this) == pc)
            return pcDesc;
    return 0;
}

std::optional<ScopeDesc> nmethod::get_scope_desc_at(uint64_t pc) const noexcept
{
    if (PcDesc pd = get_pc_desc_at(pc); pd)
        return std::make_optional<ScopeDesc>(*this, pd.get_scope_decode_offset(), pd.get_obj_decode_offset(),
                                             pd.get_reexecute());
    return std::nullopt;
}

bool nmethod::contains_method(oops::Method method_addr) const
{
    for (uint64_t p = scopes_pcs_begin(); p < scopes_pcs_end(); p += PcDesc::pc_desc_size)
    {
        PcDesc pc_desc{p};
        if (pc_desc.get_scope_decode_offset() == DebugInformationRecorder::SERIALIZED_NULL)
            continue;
        std::optional<ScopeDesc> scope = std::make_optional<ScopeDesc>(
            *this, pc_desc.get_scope_decode_offset(), pc_desc.get_obj_decode_offset(), pc_desc.get_reexecute());
        while (scope)
        {
            if (scope->get_method() == method_addr)
                return true;
            scope = scope->sender();
        }
    }
    return false;
}

#ifdef _WIN32
bool nmethod::make_not_entrant() {}
#else
__attribute__((naked)) bool nmethod::make_not_entrant()
{
    asm volatile("ldr x0, [x0]       \n\t"
                 "br  %[func]        \n\t"
                 :
                 : [func] "r"(*(void **)(nmethod_vptr + 0xf8))
                 : "x0", "memory");
}
#endif

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

    nmethod_vptr = (uint8_t *)*runtime::Jvm::get_vtbl_for_type(type);
}
} // namespace hotspot::code