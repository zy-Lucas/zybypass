#pragma once

#include "debugInformationRecorder.h"
#include "nmethod.hpp"

namespace hotspot::code
{
class ScopeDesc
{
  public:
    // ScopeDesc(nmethod code, uint32_t decode_offset, std::vector<ObjectValue> objects, bool reexecute);
    ScopeDesc(nmethod code, uint32_t decode_offset, uint32_t object_decode_offset, bool reexecute) noexcept;
    ScopeDesc(uint64_t addr) noexcept : code_(addr) {}

    nmethod code() const noexcept { return code_; }
    oops::Method method() const noexcept { return method_; }
    uint32_t bci() const noexcept { return bci_; }
    bool reexecute() const noexcept { return reexecute_; }
    uint32_t decode_offset() const noexcept { return decode_offset_; }

    ScopeDesc sender() const noexcept;

    bool is_top() const noexcept { return sender_decode_offset_ == DebugInformationRecorder::SERIALIZED_NULL; }

    bool operator==(const ScopeDesc &sd) const noexcept { return method_ == sd.method_ && bci_ == sd.bci_; }

    explicit operator bool() const noexcept { return (bool)code_; }

  private:
    nmethod code_;
    oops::Method method_{0};
    uint32_t bci_;
    bool reexecute_;

    uint32_t decode_offset_;
    uint32_t sender_decode_offset_;
    uint32_t locals_decode_offset_;
    uint32_t expressions_decode_offset_;
    uint32_t monitors_decode_offset_;
};
} // namespace hotspot::code