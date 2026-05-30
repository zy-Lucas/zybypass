#pragma once

#include "javaThread.hpp"

namespace hotspot::runtime
{
class ThreadsList : public JvmObject
{
  public:
    ThreadsList(uint64_t addr) noexcept : JvmObject(addr) {}

    uint32_t length() const noexcept { return read_field<uint32_t>(length_offset_); }

    uint64_t java_thread_address_at(uint32_t i) const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t threads_offset_;
    static inline uint64_t length_offset_;
};

class Threads
{
  public:
    static JavaThread current() noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t thread_list_offset_;

    static inline ThreadsList list_{0};
};
} // namespace hotspot::runtime