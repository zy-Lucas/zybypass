#pragma once

#include "nmethod.hpp"

namespace hotspot::code
{
class PcDesc : public runtime::JvmObject
{
  public:
    PcDesc(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    int32_t pc_offset() const noexcept { return read_field<int32_t>(pc_offset_offset_); }
    int32_t scope_decode_offset() const noexcept { return read_field<int32_t>(scope_decode_offset_offset_); }
    int32_t obj_decode_offset() const noexcept { return read_field<int32_t>(obj_decode_offset_offset_); }

    uint64_t real_pc(nmethod code) const noexcept { return code.code_begin() + pc_offset(); }

    int32_t flags() const noexcept { return read_field<int32_t>(flags_offset_); }

    bool reexecute() const noexcept { return flags() & reexecute_mask_; }
    bool is_method_handle_invoke() const noexcept { return flags() & is_method_handle_invoke_mask_; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t pc_offset_offset_;
    static inline uint64_t scope_decode_offset_offset_;
    static inline uint64_t obj_decode_offset_offset_;
    static inline uint64_t flags_offset_;

  public:
    static inline uint64_t pc_desc_size_;

  private:
    static inline int32_t reexecute_mask_;
    static inline int32_t is_method_handle_invoke_mask_;
    static inline int32_t return_oop_mask_;
};
} // namespace hotspot::code