#pragma once

#include "compiledMethod.hpp"

namespace hotspot::code
{
class PcDesc;
class ScopeDesc;

enum
{
    not_installed = -1,
    in_use = 0,
    not_used = 1,
    not_entrant = 2,
    unloaded = 3,
    zombie = 4
};

class nmethod : public CompiledMethod
{
  public:
    nmethod(uint64_t addr) noexcept : CompiledMethod(addr) {}

    bool is_osr_method() const noexcept { return entry_bci() != runtime::Jvm::invocation_entry_bci(); }

    uint64_t constants_begin() const noexcept { return content_begin(); }
    uint64_t constants_end() const noexcept { return entry_point(); }

    uint64_t insts_begin() const noexcept { return code_begin(); }
    uint64_t insts_end() const noexcept { return header_begin() + stub_offset(); }

    uint64_t stub_begin() const noexcept { return header_begin() + stub_offset(); }
    uint64_t stub_end() const noexcept { return header_begin() + oops_offset(); }

    uint64_t oops_begin() const noexcept { return header_begin() + oops_offset(); }
    uint64_t oops_end() const noexcept { return header_begin() + metadata_offset(); }

    uint64_t metadata_begin() const noexcept { return header_begin() + metadata_offset(); }
    uint64_t metadata_end() const noexcept { return scopes_data_begin(); }

    uint64_t scopes_data_end() const noexcept { return header_begin() + scopes_pcs_offset(); }

    uint64_t scopes_pcs_begin() const noexcept { return header_begin() + scopes_pcs_offset(); }
    uint64_t scopes_pcs_end() const noexcept { return header_begin() + dependencies_offset(); }

    uint64_t dependencies_begin() const noexcept { return header_begin() + dependencies_offset(); }
    uint64_t dependencies_end() const noexcept { return header_begin() + handler_table_offset(); }

    uint64_t handler_table_begin() const noexcept { return header_begin() + handler_table_offset(); }
    uint64_t handler_table_end() const noexcept { return header_begin() + nul_chk_table_offset(); }

    uint64_t nul_chk_table_begin() const noexcept { return header_begin() + nul_chk_table_offset(); }
    uint64_t nul_chk_table_end() const noexcept { return header_begin() + nmethod_end_offset(); }

    uint32_t constants_size() const noexcept { return constants_end() - constants_begin(); }
    uint32_t insts_size() const noexcept { return insts_end() - insts_begin(); }
    uint32_t stub_size() const noexcept { return stub_end() - stub_begin(); }
    uint32_t oops_size() const noexcept { return oops_end() - oops_begin(); }
    uint32_t metadata_size() const noexcept { return metadata_end() - metadata_begin(); }
    uint32_t scopes_data_size() const noexcept { return scopes_data_end() - scopes_data_begin(); }
    uint32_t scopes_pcs_size() const noexcept { return scopes_pcs_end() - scopes_pcs_begin(); }
    uint32_t dependencies_size() const noexcept { return dependencies_end() - dependencies_begin(); }
    uint32_t handler_table_size() const noexcept { return handler_table_end() - handler_table_begin(); }
    uint32_t nul_chk_table_size() const noexcept { return nul_chk_table_end() - nul_chk_table_begin(); }

    uint32_t total_size() const noexcept;

    bool is_not_installed() const noexcept { return state() == not_installed; }
    bool is_in_use() const noexcept { return state() <= in_use; }
    bool is_alive() const noexcept { return state() < unloaded; }
    bool is_not_entrant() const noexcept { return state() == not_entrant; }
    bool is_zombie() const noexcept { return state() == zombie; }
    bool is_unloaded() const noexcept { return state() == unloaded; }

    bool is_unloading();

    uint32_t oops_length() const noexcept { return oops_size() / runtime::Jvm::oop_size(); }
    uint32_t metadata_length() const noexcept { return metadata_size() / runtime::Jvm::oop_size(); }

    uint64_t entry_point() const noexcept { return read_field<uint64_t>(entry_point_offset_); }
    uint64_t verified_entry_point() const noexcept { return read_field<uint64_t>(verified_entry_point_offset_); }

    uint64_t metadata_at(uint32_t index) const noexcept;
    oops::Method get_method(uint32_t index) const noexcept;

    nmethod osr_link() const noexcept { return read_field<uint64_t>(osr_link_offset_); }

    bool is_method_handle_return(uint64_t return_pc) const noexcept;
    bool is_locked_by_vm() const noexcept { return read_field<int32_t>(lock_count_offset_) > 0; }
    bool is_deopt_pc(uint64_t pc) const noexcept { return is_deopt_entry(pc) || is_deopt_mh_entry(pc); }
    bool is_deopt_entry(uint64_t pc) const noexcept { return pc == deopt_handler_begin(); }
    bool is_deopt_mh_entry(uint64_t pc) const noexcept { return pc == deopt_mh_handler_begin(); }

    int8_t state() const noexcept { return atomic_load_field<int8_t>(state_offset_); }

    PcDesc pc_desc_at(uint64_t pc) const noexcept;
    ScopeDesc scope_desc_at(uint64_t pc) const noexcept;

    bool contains_method(oops::Method method_addr) const noexcept;

    bool make_not_entrant();

    // private:
    int32_t entry_bci() const noexcept { return read_field<int32_t>(entry_bci_offset_); }
    int32_t exception_offset() const noexcept { return read_field<int32_t>(exception_offset_offset_); }
    int32_t stub_offset() const noexcept { return read_field<int32_t>(stub_offset_offset_); }
    int32_t oops_offset() const noexcept { return read_field<int32_t>(oops_offset_offset_); }
    int32_t metadata_offset() const noexcept { return read_field<int32_t>(metadata_offset_offset_); }
    int32_t scopes_pcs_offset() const noexcept { return read_field<int32_t>(scopes_pcs_offset_offset_); }
    int32_t dependencies_offset() const noexcept { return read_field<int32_t>(dependencies_offset_offset_); }
    int32_t handler_table_offset() const noexcept { return read_field<int32_t>(handler_table_offset_offset_); }
    int32_t nul_chk_table_offset() const noexcept { return read_field<int32_t>(nul_chk_table_offset_offset_); }
    int32_t nmethod_end_offset() const noexcept { return read_field<int32_t>(nmethod_end_offset_offset_); }
    int32_t comp_level() const noexcept { return read_field<int32_t>(comp_level_offset_); }

    DECLARE_STATIC_INIT

    static inline types::Type *type_;

    static inline uint64_t entry_bci_offset_;
    static inline uint64_t osr_link_offset_;
    static inline uint64_t state_offset_;
    static inline uint64_t exception_offset_offset_;
    static inline uint64_t orig_pc_offset_offset_;
    static inline uint64_t stub_offset_offset_;
    static inline uint64_t oops_offset_offset_;
    static inline uint64_t metadata_offset_offset_;
    static inline uint64_t scopes_pcs_offset_offset_;
    static inline uint64_t dependencies_offset_offset_;
    static inline uint64_t handler_table_offset_offset_;
    static inline uint64_t nul_chk_table_offset_offset_;
    static inline uint64_t nmethod_end_offset_offset_;
    static inline uint64_t entry_point_offset_;
    static inline uint64_t verified_entry_point_offset_;
    static inline uint64_t osr_entry_point_offset_;
    static inline uint64_t lock_count_offset_;
    static inline uint64_t stack_traversal_mark_offset_;
    static inline uint64_t comp_level_offset_;

    static inline uint8_t *nmethod_vptr_;
};
} // namespace hotspot::code