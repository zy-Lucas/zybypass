#include "methodCounters.hpp"

namespace hotspot::oops
{
uint16_t MethodCounter::interpreter_throwout_count() const noexcept
{
    if (!interpreter_throwout_count_offset_)
        return 0;
    return read_field<uint16_t>(*interpreter_throwout_count_offset_);
}

uint32_t MethodCounter::invocation_counter() const noexcept
{
    if (!invocation_counter_offset_)
        return 0;
    return read_field<uint32_t>(*invocation_counter_offset_);
}

uint32_t MethodCounter::backedge_counter() const noexcept
{
    if (!backedge_counter_offset_)
        return 0;
    return read_field<uint32_t>(*backedge_counter_offset_);
}

void MethodCounter::initialize()
{
    utils::FieldResolver r{"MethodCounters"};
    
    if (runtime::Jvm::is_server_compiler())
        r.field_offset("_interpreter_throwout_count", interpreter_throwout_count_offset_);
    if (!runtime::Jvm::is_core())
    {
        r.field_offset("_invocation_counter", invocation_counter_offset_);
        r.field_offset("_backedge_counter", backedge_counter_offset_);
    }
}
} // namespace hotspot::oops