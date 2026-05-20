#include "thread.hpp"

namespace hotspot::runtime
{
void Thread::initialize()
{
    types::Type *type_thread = Jvm::lookup_type("Thread");
    types::Type *type_java_thread = Jvm::lookup_type("JavaThread");

    tlab_field_offset_ = *type_thread->field_offset("_tlab");
    active_handles_offset_ = *type_thread->field_offset("_active_handles");
    allocated_bytes_offset_ = *type_thread->field_offset("_allocated_bytes");

    has_async_exception_ = *Jvm::lookup_int_constant("Thread::_has_async_exception");

    suspend_flags_offset_ = *type_java_thread->field_offset("_suspend_flags");
    current_pending_monitor_offset_ = *type_java_thread->field_offset("_current_pending_monitor");
    current_waiting_monitor_offset_ = *type_java_thread->field_offset("_current_waiting_monitor");
}
} // namespace hotspot::runtime