#pragma once

#include "codeBlob.hpp"
#include "oops/method.hpp"

namespace hotspot::code
{
class CompiledMethod : public CodeBlob
{
  public:
    CompiledMethod(uint64_t addr) noexcept : CodeBlob(addr) {}

    bool is_marked_for_deoptimization() const noexcept;

    oops::Method method() const noexcept { return read_field<uint64_t>(method_offset_); }

    uint64_t scopes_data_begin() const noexcept { return read_field<uint64_t>(scopes_data_begin_offset_); }
    uint64_t deopt_handler_begin() const noexcept { return read_field<uint64_t>(deopt_handler_begin_offset_); }
    uint64_t deopt_mh_handler_begin() const noexcept { return read_field<uint64_t>(deopt_mh_handler_begin_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t mark_for_deoptimization_status_offset_;
    static inline uint64_t method_offset_;
    static inline uint64_t scopes_data_begin_offset_;
    static inline uint64_t deopt_handler_begin_offset_;
    static inline uint64_t deopt_mh_handler_begin_offset_;
};
} // namespace hotspot::code