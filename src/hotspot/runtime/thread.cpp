#include "thread.hpp"

namespace hotspot::runtime
{
Thread::Thread(uint64_t addr) : JvmObject<Thread>(addr) {}

void Thread::initialize()
{
    types::Type *type_thread = Jvm::lookup_type("Thread");
    types::Type *type_java_thread = Jvm::lookup_type("JavaThread");

    suspend_flags_offset = type_java_thread->get_field("_suspend_flags")->get_offset();
    HAS_ASYNC_EXCEPTION = Jvm::lookup_int_constant("Thread::HAS_ASYNC_EXCEPTION").value_or(0);

    tlab_field_offset = type_thread->get_field("_tlab")->get_offset();
    active_handles_offset = type_thread->get_field("_active_handles")->get_offset();
    current_pending_monitor_offset = type_java_thread->get_field("_current_pending_monitor")->get_offset();
    current_waiting_monitor_offset = type_java_thread->get_field("_current_waiting_monitor")->get_offset();
    allocated_bytes_offset = type_thread->get_field("_allocated_bytes")->get_offset();
}
} // namespace hotspot::runtime