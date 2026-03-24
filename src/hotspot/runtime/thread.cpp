#include "thread.hpp"

namespace hotspot::runtime
{
Thread::Thread(uint64_t addr) : JvmObject<Thread>(addr) {}

void Thread::initialize()
{
    types::Type *type_thread = Jvm::lookup_type("Thread");
    types::Type *type_java_thread = Jvm::lookup_type("JavaThread");

    suspend_flags_offset = type_java_thread->get_field_offset("_suspend_flags").value_or(0);
    HAS_ASYNC_EXCEPTION = Jvm::lookup_int_constant("Thread::HAS_ASYNC_EXCEPTION").value_or(0);

    tlab_field_offset = type_thread->get_field_offset("_tlab").value_or(0);
    active_handles_offset = type_thread->get_field_offset("_active_handles").value_or(0);
    current_pending_monitor_offset = type_java_thread->get_field_offset("_current_pending_monitor").value_or(0);
    current_waiting_monitor_offset = type_java_thread->get_field_offset("_current_waiting_monitor").value_or(0);
    allocated_bytes_offset = type_thread->get_field_offset("_allocated_bytes").value_or(0);
}
} // namespace hotspot::runtime