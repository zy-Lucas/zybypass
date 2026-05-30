#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::interpreter
{
class InterpreterCodelet : public runtime::JvmObject
{
  public:
    InterpreterCodelet(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    int32_t size() const noexcept { return read_field<int32_t>(size_offset_); }

    std::string_view description_view() const noexcept { return read_string_field(description_offset_); }
    std::string description() const noexcept { return std::string{description_view()}; }

    uint64_t code_begin() const noexcept;
    uint64_t code_end() const noexcept { return address() + size(); }
    uint64_t code_size() const noexcept { return code_end() - code_begin(); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t description_offset_;
    static inline uint64_t size_offset_;
    static inline uint64_t bytecode_offset_;

    static inline uint64_t instance_size_;
};
} // namespace hotspot::interpreter