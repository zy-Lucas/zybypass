#include "methodCounter.hpp"

namespace hotspot::oops
{
MethodCounter::MethodCounter(uint64_t addr) : MetaData(addr) { STATIC_INIT_GUARD; }

void MethodCounter::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("MethodCounter");

    if (runtime::Jvm::is_server_compiler())
        interpreter_throwout_count_offset = type->get_field_offset("_interpreter_throwout_count");
    if (!runtime::Jvm::is_core())
    {
        invocation_counter_offset = type->get_field_offset("_invocation_counter");
        backedge_counter_offset = type->get_field_offset("_backedge_counter");
    }
}
} // namespace hotspot::oops