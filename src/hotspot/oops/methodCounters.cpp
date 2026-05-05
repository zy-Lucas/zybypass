#include "methodCounters.hpp"

namespace hotspot::oops
{
uint16_t MethodCounter::get_interpreter_throwout_count() const noexcept
{
    if (!interpreter_throwout_count_offset)
        return 0;
    return read_field<uint16_t>(*interpreter_throwout_count_offset);
}

uint32_t MethodCounter::get_invocation_counter() const noexcept
{
    if (!invocation_counter_offset)
        return 0;
    return read_field<uint32_t>(*invocation_counter_offset);
}

uint32_t MethodCounter::get_backedge_counter() const noexcept
{
    if (!backedge_counter_offset)
        return 0;
    return read_field<uint32_t>(*backedge_counter_offset);
}

void MethodCounter::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("MethodCounters");

    if (runtime::Jvm::is_server_compiler())
        interpreter_throwout_count_offset = type->get_field_offset("_interpreter_throwout_count");
    if (!runtime::Jvm::is_core())
    {
        invocation_counter_offset = type->get_field_offset("_invocation_counter");
        backedge_counter_offset = type->get_field_offset("_backedge_counter");
    }
}
} // namespace hotspot::oops