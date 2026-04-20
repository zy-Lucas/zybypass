#pragma once

#include "nmethod.hpp"

namespace hotspot::code
{
class PcDesc : public runtime::JvmObject
{
  public:
    PcDesc(uint64_t addr);

    int32_t get_pc_offset() const noexcept { return read_field<int32_t>(pc_offset_offset); }
    int32_t get_scope_decode_offset() const noexcept { return read_field<int32_t>(scope_decode_offset_offset); }
    int32_t get_obj_decode_offset() const noexcept { return read_field<int32_t>(obj_decode_offset_offset); }

    uint64_t get_real_pc(nmethod code) const noexcept { return code.code_begin() + get_pc_offset(); }

    int32_t get_flags() const noexcept { return read_field<int32_t>(flags_offset); }

    bool get_reexecute() const noexcept { return get_flags() & reexecute_mask; }
    bool is_method_handle_invoke() const noexcept { return get_flags() & is_method_handle_invoke_mask; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t pc_offset_offset;
    static inline uint64_t scope_decode_offset_offset;
    static inline uint64_t obj_decode_offset_offset;
    static inline uint64_t flags_offset;

    static inline int32_t reexecute_mask;
    static inline int32_t is_method_handle_invoke_mask;
    static inline int32_t return_oop_mask;

  public:
    static inline uint32_t pc_desc_size;
};
} // namespace hotspot::code