#pragma once

#include "compiledMethod.hpp"
#include <cstdint>

namespace hotspot::code
{
class nmethod : public CompiledMethod
{
  public:
    nmethod(uint64_t addr);

    bool is_osr_method() const noexcept { return get_entry_bci() != runtime::Jvm::get_invocation_entry_bic(); }

    uint64_t constants_begin() const noexcept { return content_begin(); }
    uint64_t constants_end() const noexcept { return get_entry_point(); }

    uint64_t insts_begin() const noexcept { return code_begin(); }
    uint64_t insts_end() const noexcept { return header_begin() + get_stub_offset(); }

    uint64_t stub_begin() const noexcept { return header_begin() + get_stub_offset(); }
    uint64_t stub_end() const noexcept { return header_begin() + get_oops_offset(); }

    uint64_t oops_begin() const noexcept { return header_begin() + get_oops_offset(); }
    uint64_t oops_end() const noexcept { return header_begin() + get_metadata_offset(); }

    uint64_t metadata_begin() const noexcept { return header_begin() + get_metadata_offset(); }
    uint64_t metadata_end() const noexcept { return scopes_data_begin(); }

    uint64_t scopes_data_end() const noexcept { return header_begin() + get_scopes_pcs_offset(); }

    uint64_t scopes_pcs_begin() const noexcept { return header_begin() + get_scopes_pcs_offset(); }
    uint64_t scopes_pcs_end() const noexcept { return header_begin() + get_dependencies_offset(); }

    uint64_t dependencies_begin() const noexcept { return header_begin() + get_dependencies_offset(); }
    uint64_t dependencies_end() const noexcept { return header_begin() + get_handler_table_offset(); }

    uint64_t handler_table_begin() const noexcept { return header_begin() + get_handler_table_offset(); }
    uint64_t handler_table_end() const noexcept { return header_begin() + get_nul_chk_table_offset(); }

    uint64_t nul_chk_table_begin() const noexcept { return header_begin() + get_nul_chk_table_offset(); }
    uint64_t nul_chk_table_end() const noexcept { return header_begin() + get_nmethod_end_offset(); }

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

    uint32_t get_oops_length() const noexcept { return oops_size() / *runtime::Jvm::get_oop_size(); }
    uint32_t get_metadata_length() const noexcept { return metadata_size() / *runtime::Jvm::get_oop_size(); }

    uint64_t get_entry_point() const noexcept { return read_field<uint64_t>(entry_point_offset); }
    uint64_t get_verified_entry_point() const noexcept { return read_field<uint64_t>(verified_entry_point_offset); }

    uint64_t get_metadata_at(uint32_t index) const noexcept;
    oops::Method get_method(uint32_t index) const;

    nmethod get_osr_link() const noexcept { return read_field<uint64_t>(osr_link_offset); }

    bool is_locked_by_vm() const noexcept { return read_field<int32_t>(lock_count_offset) > 0; }

  private:
    int32_t get_entry_bci() const noexcept { return read_field<int32_t>(entry_bci_offset); }
    int32_t get_exception_offset() const noexcept { return read_field<int32_t>(exception_offset_offset); }
    int32_t get_stub_offset() const noexcept { return read_field<int32_t>(stub_offset_offset); }
    int32_t get_oops_offset() const noexcept { return read_field<int32_t>(oops_offset_offset); }
    int32_t get_metadata_offset() const noexcept { return read_field<int32_t>(metadata_offset_offset); }
    int32_t get_scopes_pcs_offset() const noexcept { return read_field<int32_t>(scopes_pcs_offset_offset); }
    int32_t get_dependencies_offset() const noexcept { return read_field<int32_t>(dependencies_offset_offset); }
    int32_t get_handler_table_offset() const noexcept { return read_field<int32_t>(handler_table_offset_offset); }
    int32_t get_nul_chk_table_offset() const noexcept { return read_field<int32_t>(nul_chk_table_offset_offset); }
    int32_t get_nmethod_end_offset() const noexcept { return read_field<int32_t>(nmethod_end_offset_offset); }
    int32_t get_comp_level() const noexcept { return read_field<int32_t>(comp_level_offset); }

    DECLARE_STATIC_INIT

    static inline uint64_t entry_bci_offset;
    static inline uint64_t osr_link_offset;
    static inline uint64_t state_offset;
    static inline uint64_t exception_offset_offset;
    static inline uint64_t orig_pc_offset_offset;
    static inline uint64_t stub_offset_offset;
    static inline uint64_t oops_offset_offset;
    static inline uint64_t metadata_offset_offset;
    static inline uint64_t scopes_pcs_offset_offset;
    static inline uint64_t dependencies_offset_offset;
    static inline uint64_t handler_table_offset_offset;
    static inline uint64_t nul_chk_table_offset_offset;
    static inline uint64_t nmethod_end_offset_offset;
    static inline uint64_t entry_point_offset;
    static inline uint64_t verified_entry_point_offset;
    static inline uint64_t osr_entry_point_offset;
    static inline uint64_t lock_count_offset;
    static inline uint64_t stack_traversal_mark_offset;
    static inline uint64_t comp_level_offset;
};
} // namespace hotspot::code