#pragma once

#include "metaData.hpp"

namespace hotspot::oops
{
class MethodCounter : public MetaData
{
  public:
    MethodCounter(uint64_t addr);

    uint16_t get_interpreter_throwout_count() const noexcept
    {
        return interpreter_throwout_count_offset ? read_field<uint16_t>(*interpreter_throwout_count_offset) : 0;
    }

    uint32_t get_invocation_counter() const noexcept
    {
        return invocation_counter_offset ? read_field<uint32_t>(*invocation_counter_offset) : 0;
    }

    uint32_t get_backedge_counter() const noexcept
    {
        return backedge_counter_offset ? read_field<uint32_t>(*backedge_counter_offset) : 0;
    }

  private:
    DECLARE_STATIC_INIT

    static inline std::optional<uint64_t> interpreter_throwout_count_offset;
    static inline std::optional<uint64_t> invocation_counter_offset;
    static inline std::optional<uint64_t> backedge_counter_offset;
};
} // namespace hotspot::oops