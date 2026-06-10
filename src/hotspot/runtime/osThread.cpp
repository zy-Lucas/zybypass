#include "osThread.hpp"

namespace hotspot::runtime
{
void OSThread::initialize()
{
    utils::FieldResolver r{"OSThread"};

    r.field_offset("_thread_id", thread_id_offset_);
    r.field_offset("_state", state_offset_);

    utils::constants::int_const("ALLOCATED", allocated_);
    utils::constants::int_const("INITIALIZED", initialized_);
    utils::constants::int_const("RUNNABLE", runnable_);
    utils::constants::int_const("MONITOR_WAIT", monitor_wait_);
    utils::constants::int_const("CONDVAR_WAIT", condvar_wait_);
    utils::constants::int_const("OBJECT_WAIT", object_wait_);
    utils::constants::int_const("BREAKPOINTED", breakpointed_);
    utils::constants::int_const("SLEEPING", sleeping_);
    utils::constants::int_const("ZOMBIE", zombie_);
}
} // namespace hotspot::runtime