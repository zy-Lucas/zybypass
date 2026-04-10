#pragma once

#include "../oops/method.hpp"
#include "codeBlob.hpp"

namespace hotspot::code
{
class CompiledMethod : public CodeBlob
{
  public:
    CompiledMethod(uint64_t addr);

    oops::Method get_method() const noexcept { return read_field<uint64_t>(method_offset); }

    uint64_t scopes_data_begin() const noexcept { return read_field<uint64_t>(scopes_data_begin_offset); }
    uint64_t deopt_handler_begin() const noexcept { return read_field<uint64_t>(deopt_handler_begin_offset); }
    uint64_t deopt_mh_handler_begin() const noexcept { return read_field<uint64_t>(deopt_mh_handler_begin_offset); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t method_offset;
    static inline uint64_t scopes_data_begin_offset;
    static inline uint64_t deopt_handler_begin_offset;
    static inline uint64_t deopt_mh_handler_begin_offset;
};
} // namespace hotspot::code