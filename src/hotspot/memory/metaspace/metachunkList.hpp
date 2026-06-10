#pragma once

#include "memory/metaspace/counters.hpp"
#include "runtime/jvmObject.hpp"

namespace hotspot::memory::metaspace
{
class MetachunkList : runtime::JvmObject
{
  public:
    MetachunkList(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    IntCounter *num_chunks() const noexcept { return (IntCounter *)(address() + num_chunks_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t first_offset_;
    static inline uint64_t num_chunks_offset_;
};
} // namespace hotspot::memory::metaspace