#include "thread.hpp"

namespace hotspot::runtime
{
Thread::Thread(uint64_t addr) : JvmObject<Thread>(addr) {}

void Thread::initialize()
{
    types::Type *type_thread = Jvm::lookup_type("Thread");
    types::Type *type_java_thread = Jvm::lookup_type("JavaThread");

    suspend_flags_offset = *type_java_thread->get_field_offset("_suspend_flags");
    HAS_ASYNC_EXCEPTION = *Jvm::lookup_int_constant("Thread::_has_async_exception");

    tlab_field_offset = *type_thread->get_field_offset("_tlab");
    active_handles_offset = *type_thread->get_field_offset("_active_handles");
    current_pending_monitor_offset = *type_java_thread->get_field_offset("_current_pending_monitor");
    current_waiting_monitor_offset = *type_java_thread->get_field_offset("_current_waiting_monitor");
    allocated_bytes_offset = *type_thread->get_field_offset("_allocated_bytes");
}
} // namespace hotspot::runtime