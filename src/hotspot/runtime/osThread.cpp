#include "osThread.hpp"

namespace hotspot::runtime
{
void OSThread::initialize()
{
    types::Type *type = Jvm::lookup_type("OSThread");

    thread_id_offset_ = *type->field_offset("_thread_id");
    state_offset_ = *type->field_offset("_state");

    allocated_ = *Jvm::lookup_int_constant("ALLOCATED");
    initialized_ = *Jvm::lookup_int_constant("INITIALIZED");
    runnable_ = *Jvm::lookup_int_constant("RUNNABLE");
    monitor_wait_ = *Jvm::lookup_int_constant("MONITOR_WAIT");
    condvar_wait_ = *Jvm::lookup_int_constant("CONDVAR_WAIT");
    object_wait_ = *Jvm::lookup_int_constant("OBJECT_WAIT");
    breakpointed_ = *Jvm::lookup_int_constant("BREAKPOINTED");
    sleeping_ = *Jvm::lookup_int_constant("SLEEPING");
    zombie_ = *Jvm::lookup_int_constant("ZOMBIE");
}
} // namespace hotspot::runtime