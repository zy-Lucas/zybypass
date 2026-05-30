#pragma once

#include "jvmObject.hpp"

namespace hotspot::runtime
{
class OSThread : public JvmObject
{
  public:
    OSThread(uint64_t addr) : JvmObject(addr) {}

    int32_t thread_id() const noexcept { return read_field<int32_t>(thread_id_offset_); }
  private:
    DECLARE_STATIC_INIT

    static inline uint64_t thread_id_offset_;
    static inline uint64_t state_offset_;

    static inline int32_t allocated_;
    static inline int32_t initialized_;
    static inline int32_t runnable_;
    static inline int32_t monitor_wait_;
    static inline int32_t condvar_wait_;
    static inline int32_t object_wait_;
    static inline int32_t breakpointed_;
    static inline int32_t sleeping_;
    static inline int32_t zombie_;
};
} // namespace hotspot::runtime