#pragma once

#include "metaData.hpp"

namespace hotspot::oops
{
class MethodCounter : public MetaData
{
  public:
    MethodCounter(uint64_t addr) noexcept : MetaData(addr) {}

    uint16_t interpreter_throwout_count() const noexcept;
    uint32_t invocation_counter() const noexcept;
    uint32_t backedge_counter() const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline std::optional<uint64_t> interpreter_throwout_count_offset_;
    static inline std::optional<uint64_t> invocation_counter_offset_;
    static inline std::optional<uint64_t> backedge_counter_offset_;
};
} // namespace hotspot::oops