#pragma once

#include "jni.h"
#include "oops/vmOopHandle.hpp"
#include "osThread.hpp"
#include "thread.hpp"

namespace hotspot::runtime
{
class JavaThread : public Thread
{
  public:
    JavaThread(uint64_t addr) noexcept : Thread(addr) {}

    OSThread osthread() const noexcept { return read_field<uint64_t>(osthread_offset_); }

    oops::Instance thread_obj() const { return oops::VMOopHandle{address() + thread_obj_offset_}.resolve(); }

    JNIEnv *jni_environment() const noexcept { return (JNIEnv *)(address() + jni_environment_offset_); }

    uint64_t stack_base() const noexcept { return read_field<uint64_t>(stack_base_offset_); }
    uint64_t stack_size() const noexcept { return read_field<uint64_t>(stack_size_offset_); }

    uint64_t stack_base_value() const noexcept { return Jvm::read<uint64_t>(stack_base()); }

    uint32_t suspend_flags() const noexcept { return read_field<uint32_t>(suspend_flags_offset_); }
    bool has_async_exception() const noexcept { return suspend_flags() & has_async_exception_; }

    int32_t terminated() const noexcept { return read_field<int32_t>(terminated_offset_); }

    bool is_exiting() const noexcept { return terminated() == thread_exiting_ || is_terminated(); }
    bool is_terminated() const noexcept { return terminated() != not_terminated_ && terminated() != thread_exiting_; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t osthread_offset_;
    static inline uint64_t thread_obj_offset_;
    static inline uint64_t anchor_offset_;
    static inline uint64_t jni_environment_offset_;
    static inline uint64_t stack_base_offset_;
    static inline uint64_t stack_size_offset_;
    static inline uint64_t current_pending_monitor_offset_;
    static inline uint64_t current_waiting_monitor_offset_;
    static inline uint64_t suspend_flags_offset_;
    static inline uint64_t thread_state_offset_;
    static inline uint64_t terminated_offset_;

    static inline uint64_t last_java_sp_offset_;
    static inline uint64_t last_java_pc_offset_;

    static inline int32_t thread_uninitialized_;
    static inline int32_t thread_new_;
    static inline int32_t thread_new_trans_;
    static inline int32_t thread_in_native_;
    static inline int32_t thread_in_native_trans_;
    static inline int32_t thread_in_vm_;
    static inline int32_t thread_in_vm_trans_;
    static inline int32_t thread_in_java_;
    static inline int32_t thread_in_java_trans_;
    static inline int32_t thread_blocked_;
    static inline int32_t thread_blocked_trans_;

    static inline int32_t has_async_exception_;
    static inline int32_t not_terminated_;
    static inline int32_t thread_exiting_;
};
} // namespace hotspot::runtime