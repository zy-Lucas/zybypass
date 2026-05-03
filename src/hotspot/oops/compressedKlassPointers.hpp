#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::oops
{
class CompressedKlassPointers
{
  public:
    static uint64_t get_base() noexcept { return runtime::Jvm::read<uint64_t>(base_offset); }
    static int32_t get_shift() noexcept { return runtime::Jvm::read<int32_t>(shift_offset); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t base_offset;
    static inline uint64_t shift_offset;
};
} // namespace hotspot::oops