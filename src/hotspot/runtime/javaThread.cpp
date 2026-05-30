#include "javaThread.hpp"
#include "jvm.hpp"

namespace hotspot::runtime
{
void JavaThread::initialize()
{
    types::Type *type = Jvm::lookup_type("JavaThread");
    types::Type *anchorType = Jvm::lookup_type("JavaFrameAnchor");

    osthread_offset_ = *type->field_offset("_osthread");
    stack_base_offset_ = *type->field_offset("_stack_base");
    stack_size_offset_ = *type->field_offset("_stack_size");
    thread_obj_offset_ = *type->field_offset("_threadObj");
    anchor_offset_ = *type->field_offset("_anchor");
    current_pending_monitor_offset_ = *type->field_offset("_current_pending_monitor");
    current_waiting_monitor_offset_ = *type->field_offset("_current_waiting_monitor");
    suspend_flags_offset_ = *type->field_offset("_suspend_flags");
    thread_state_offset_ = *type->field_offset("_thread_state");
    terminated_offset_ = *type->field_offset("_terminated");

    last_java_sp_offset_ = *anchorType->field_offset("_last_Java_sp");
    last_java_pc_offset_ = *anchorType->field_offset("_last_Java_pc");

    thread_uninitialized_ = *Jvm::lookup_int_constant("_thread_uninitialized");
    thread_new_ = *Jvm::lookup_int_constant("_thread_new");
    thread_new_trans_ = *Jvm::lookup_int_constant("_thread_new_trans");
    thread_in_native_ = *Jvm::lookup_int_constant("_thread_in_native");
    thread_in_native_trans_ = *Jvm::lookup_int_constant("_thread_in_native_trans");
    thread_in_vm_ = *Jvm::lookup_int_constant("_thread_in_vm");
    thread_in_vm_trans_ = *Jvm::lookup_int_constant("_thread_in_vm_trans");
    thread_in_java_ = *Jvm::lookup_int_constant("_thread_in_Java");
    thread_in_java_trans_ = *Jvm::lookup_int_constant("_thread_in_Java_trans");
    thread_blocked_ = *Jvm::lookup_int_constant("_thread_blocked");
    thread_blocked_trans_ = *Jvm::lookup_int_constant("_thread_blocked_trans");

    has_async_exception_ = *Jvm::lookup_int_constant("JavaThread::_has_async_exception");
    not_terminated_ = *Jvm::lookup_int_constant("JavaThread::_not_terminated");
    thread_exiting_ = *Jvm::lookup_int_constant("JavaThread::_thread_exiting");
}
} // namespace hotspot::runtime