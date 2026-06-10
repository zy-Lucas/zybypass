#pragma once

#include "symbol.hpp"

namespace hotspot::oops
{
class vmSymbols
{
  public:
    static Symbol symbol_at(uint32_t id) noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t symbols_address_;

    static inline int32_t first_sid_;
    static inline int32_t sid_limit_;
};
} // namespace hotspot::oSID_LIMITops