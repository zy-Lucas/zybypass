#pragma once

#include "jvmObject.hpp"

namespace hotspot::runtime
{
class Thread : public JvmObject<Thread>
{
  public:
    Thread(uint64_t addr);

    uint32_t suspend_flags() const noexcept { return read_field<uint32_t>(suspend_flags_offset); }
    bool has_async_exception() const noexcept { return (suspend_flags() & HAS_ASYNC_EXCEPTION) != 0; }

    int64_t allocated_bytes() const noexcept { return read_field<int64_t>(allocated_bytes_offset); }

    bool is_vm_thread() const noexcept { return false; }
    bool is_java_thread() const noexcept { return false; }
    bool is_compiler_thread() const noexcept { return false; }
    bool is_code_cache_sweeper_thread() const noexcept { return false; }
    bool is_hidden_form_external_view() const noexcept { return false; }
    bool is_jvmti_agent_thread() const noexcept { return false; }
    bool is_watcher_thread() const noexcept { return false; }
    bool is_service_thread() const noexcept { return false; }
    bool is_monitor_deflation_thread() const noexcept { return false; }

  private:
    friend class JvmObject<Thread>;

    static inline std::once_flag init_flag_;

    static inline uint64_t suspend_flags_offset;
    static inline uint32_t HAS_ASYNC_EXCEPTION;

    static inline uint64_t tlab_field_offset;
    static inline uint64_t active_handles_offset;
    static inline uint64_t current_pending_monitor_offset;
    static inline uint64_t current_waiting_monitor_offset;
    static inline uint64_t allocated_bytes_offset;

    static void initialize();
};
} // namespace hotspot::runtime