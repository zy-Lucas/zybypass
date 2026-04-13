#pragma once

#include "debugInformationRecorder.h"
#include "nmethod.hpp"

namespace hotspot::code
{
class ScopeDesc
{
  public:
    // ScopeDesc(nmethod code, uint32_t decode_offset, std::vector<ObjectValue> objects, bool reexecute);
    ScopeDesc(nmethod code, uint32_t decode_offset, uint32_t object_decode_offset, bool reexecute);

    nmethod get_nmethod() const noexcept { return code; }
    oops::Method get_method() const noexcept { return method; }
    uint32_t get_bci() const noexcept { return bci; }
    bool get_reexecute() const noexcept { return reexecute; }
    uint32_t get_decode_offset() const noexcept { return decode_offset; }

    std::optional<ScopeDesc> sender() const noexcept;

    bool is_top() const noexcept { return sender_decode_offset == DebugInformationRecorder::SERIALIZED_NULL; }

    bool operator==(const ScopeDesc &sd) const noexcept { return method == sd.method && bci == sd.bci; }
    bool operator!=(const ScopeDesc &sd) const noexcept { return !operator==(sd); }

  private:
    nmethod code;
    oops::Method method{0};
    uint32_t bci;
    bool reexecute;

    int32_t decode_offset;
    uint32_t sender_decode_offset;
    uint32_t locals_decode_offset;
    uint32_t expressions_decode_offset;
    uint32_t monitors_decode_offset;
};
} // namespace hotspot::code