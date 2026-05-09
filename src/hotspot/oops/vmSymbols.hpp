#pragma once

#include "../runtime/jvmObject.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
class vmSymbols
{
  public:
    static Symbol symbol_at(uint32_t id) noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t symbols_address;

    static inline int32_t FIRST_SID;
    static inline int32_t SID_LIMIT;
};
} // namespace hotspot::oops