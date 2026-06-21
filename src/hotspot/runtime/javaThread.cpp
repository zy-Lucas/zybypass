#include "javaThread.hpp"
#include "jvm.hpp"

namespace hotspot::runtime
{
void JavaThread::initialize()
{
    utils::FieldResolver r{"JavaThread"};

    r.field_offset("_osthread", osthread_offset_);
    r.field_offset("_threadObj", thread_obj_offset_);
    r.field_offset("_anchor", anchor_offset_);
    r.field_offset("_stack_base", stack_base_offset_);
    r.field_offset("_stack_size", stack_size_offset_);
    r.field_offset("_current_pending_monitor", current_pending_monitor_offset_);
    r.field_offset("_current_waiting_monitor", current_waiting_monitor_offset_);
    r.field_offset("_suspend_flags", suspend_flags_offset_);
    r.field_offset("_thread_state", thread_state_offset_);
    r.field_offset("_terminated", terminated_offset_);

    utils::FieldResolver r_anchor{"JavaFrameAnchor"};

    r_anchor.field_offset("_last_Java_sp", last_java_sp_offset_);
    r_anchor.field_offset("_last_Java_pc", last_java_pc_offset_);

    utils::constants::int_const("_thread_uninitialized", thread_uninitialized_);
    utils::constants::int_const("_thread_new", thread_new_);
    utils::constants::int_const("_thread_new_trans", thread_new_trans_);
    utils::constants::int_const("_thread_in_native", thread_in_native_);
    utils::constants::int_const("_thread_in_native_trans", thread_in_native_trans_);
    utils::constants::int_const("_thread_in_vm", thread_in_vm_);
    utils::constants::int_const("_thread_in_vm_trans", thread_in_vm_trans_);
    utils::constants::int_const("_thread_in_Java", thread_in_java_);
    utils::constants::int_const("_thread_in_Java_trans", thread_in_java_trans_);
    utils::constants::int_const("_thread_blocked", thread_blocked_);
    utils::constants::int_const("_thread_blocked_trans", thread_blocked_trans_);

    utils::constants::int_const("JavaThread::_has_async_exception", has_async_exception_);
    utils::constants::int_const("JavaThread::_not_terminated", not_terminated_);
    utils::constants::int_const("JavaThread::_thread_exiting", thread_exiting_);

    jni_environment_offset_ = anchor_offset_ + r_anchor.type_size() + sizeof(void *);
}
} // namespace hotspot::runtime