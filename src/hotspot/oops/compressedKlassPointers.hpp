#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::oops
{
class CompressedKlassPointers
{
  public:
    static uint64_t base() noexcept { return runtime::Jvm::read<uint64_t>(base_offset_); }
    static int32_t shift() noexcept { return runtime::Jvm::read<int32_t>(shift_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t base_offset_;
    static inline uint64_t shift_offset_;
};
} // namespace hotspot::oops