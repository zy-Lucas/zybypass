#pragma once

#include "metaData.hpp"

namespace hotspot::oops
{
class MethodCounter : public MetaData
{
  public:
    MethodCounter(uint64_t addr) : MetaData(addr) {}

    uint16_t get_interpreter_throwout_count() const noexcept;
    uint32_t get_invocation_counter() const noexcept;
    uint32_t get_backedge_counter() const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline std::optional<uint64_t> interpreter_throwout_count_offset;
    static inline std::optional<uint64_t> invocation_counter_offset;
    static inline std::optional<uint64_t> backedge_counter_offset;
};
} // namespace hotspot::oops