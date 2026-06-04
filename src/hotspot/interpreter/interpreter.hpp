#pragma once

#include "code/stubQueue.hpp"
#include "runtime/jvmObject.hpp"

namespace hotspot::interpreter
{
class Interpreter
{
  public:
    static code::StubQueue code() noexcept { return runtime::Jvm::read<uint64_t>(code_offset_); }

    static bool contains(uint64_t pc) noexcept { return code().contains(pc); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t code_offset_;
};
} // namespace hotspot::interpreter