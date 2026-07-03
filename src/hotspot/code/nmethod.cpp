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

#ifdef _WIN32
__attribute__((naked)) bool nmethod::is_unloading()
{
    asm volatile("movq (%%rcx), %%rcx \n\t"
                 "jmp *%[func]        \n\t"
                 :
                 : [func] "r"(*(void **)(nmethod_vptr_ + 0x220))
                 : "rcx", "memory");
}
#else
__attribute__((naked)) bool nmethod::is_unloading()
{
    asm volatile("ldr x0, [x0]       \n\t"
                 "br  %[func]        \n\t"
                 :
                 : [func] "r"(*(void **)(nmethod_vptr_ + 0x220))
                 : "x0", "memory");
}
#endif

uint64_t nmethod::metadata_at(uint32_t index) const noexcept
{
    if (!index)
        return 0;
    return runtime::Jvm::read<uint64_t>((index - 1) * runtime::Jvm::oop_size() + metadata_begin());
}

oops::Method nmethod::get_method(uint32_t index) const noexcept
{
    // auto [name, base]{oops::MetaData::instantiate_wrapper_for(get_metadata_at(index))};
    // if (name != "Method")
    //     throw std::bad_cast();
    return metadata_at(index);
}

bool nmethod::is_method_handle_return(uint64_t return_pc) const noexcept
{
    if (PcDesc pc_desc{pc_desc_at(return_pc)}; pc_desc)
        return pc_desc.is_method_handle_invoke();
    return false;
}

PcDesc nmethod::pc_desc_at(uint64_t pc) const noexcept
{
    if (pc < code_begin())
        return 0;
    for (uint64_t p = scopes_pcs_begin(); p < scopes_pcs_end(); p += PcDesc::pc_desc_size_)
        if (PcDesc pc_desc{p}; pc_desc.real_pc(*this) == pc)
            return pc_desc;
    return 0;
}

ScopeDesc nmethod::scope_desc_at(uint64_t pc) const noexcept
{
    if (PcDesc pd{pc_desc_at(pc)}; pd)
        return {*this, (uint32_t)pd.scope_decode_offset(), (uint32_t)pd.obj_decode_offset(), pd.reexecute()};
    return 0;
}

bool nmethod::contains_method(oops::Method method_addr) const noexcept
{
    uint64_t pc_begin = scopes_pcs_begin();
    uint64_t pc_end = scopes_pcs_end();

    if (!pc_begin || !pc_end || pc_begin >= pc_end)
        return false;
    if (!PcDesc::pc_desc_size_)
        return false;

    uint64_t data_begin = scopes_data_begin();
    uint32_t data_size = scopes_data_size();
    if (!data_begin || !data_size)
        return false;
    if (data_begin & 0x7)
        return false;
    if (pc_begin % PcDesc::pc_desc_size_)
        return false;

    for (uint64_t p = pc_begin; p < pc_end; p += PcDesc::pc_desc_size_)
    {
        PcDesc pc_desc{p};
        if (pc_desc.scope_decode_offset() == DebugInformationRecorder::SERIALIZED_NULL)
            continue;

        uint32_t scope_off = pc_desc.scope_decode_offset();
        uint32_t obj_off = pc_desc.obj_decode_offset();

        if (scope_off + 56 > data_size || obj_off >= data_size)
            continue;

        ScopeDesc scope{*this, scope_off, obj_off, pc_desc.reexecute()};
        if (!scope)
            continue;
        while (scope)
        {
            if (scope.method() == method_addr)
                return true;
            scope = scope.sender();
        }
    }
    return false;
}

#ifdef _WIN32
__attribute__((naked)) bool nmethod::make_not_entrant()
{
    asm volatile("movq (%%rcx), %%rcx \n\t"
                 "jmp *%[func]        \n\t"
                 :
                 : [func] "r"(*(void **)(nmethod_vptr_ + 0xf8))
                 : "rcx", "memory");
}
#else
__attribute__((naked)) bool nmethod::make_not_entrant()
{
    asm volatile("ldr x0, [x0]       \n\t"
                 "br  %[func]        \n\t"
                 :
                 : [func] "r"(*(void **)(nmethod_vptr_ + 0xf8))
                 : "x0", "memory");
}
#endif

void nmethod::initialize()
{
    utils::FieldResolver r{"nmethod"};

    r.field_offset("_entry_bci", entry_bci_offset_);
    r.field_offset("_osr_link", osr_link_offset_);
    r.field_offset("_state", state_offset_);
    r.field_offset("_exception_offset", exception_offset_offset_);
    r.field_offset("_orig_pc_offset", orig_pc_offset_offset_);
    r.field_offset("_stub_offset", stub_offset_offset_);
    r.field_offset("_oops_offset", oops_offset_offset_);
    r.field_offset("_metadata_offset", metadata_offset_offset_);
    r.field_offset("_scopes_pcs_offset", scopes_pcs_offset_offset_);
    r.field_offset("_dependencies_offset", dependencies_offset_offset_);
    r.field_offset("_handler_table_offset", handler_table_offset_offset_);
    r.field_offset("_nul_chk_table_offset", nul_chk_table_offset_offset_);
    r.field_offset("_nmethod_end_offset", nmethod_end_offset_offset_);
    r.field_offset("_entry_point", entry_point_offset_);
    r.field_offset("_verified_entry_point", verified_entry_point_offset_);
    r.field_offset("_osr_entry_point", osr_entry_point_offset_);
    r.field_offset("_lock_count", lock_count_offset_);
    r.field_offset("_stack_traversal_mark", stack_traversal_mark_offset_);
    r.field_offset("_comp_level", comp_level_offset_);

    nmethod_vptr_ = (uint8_t *)*runtime::Jvm::vtbl_for_type(r.type());
}
} // namespace hotspot::code