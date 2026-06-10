#pragma once

#include "runtime/jvmObject.hpp"

namespace hotspot::memory::metaspace
{
class MetachunkList : runtime::JvmObject
{
  public:
    MetachunkList(uint64_t addr) : runtime::JvmObject(addr) {}

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t first_offset_;
    static inline uint64_t num_chunks_offset_;
};
} // namespace hotspot::memory::metaspace