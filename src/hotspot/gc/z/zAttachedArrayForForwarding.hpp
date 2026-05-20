#pragma once

#include "../../runtime/jvmObject.hpp"
#include "zForwardingEntry.hpp"

namespace hotspot::gc::z
{
class ZForwarding;

class ZAttachedArrayForForwarding : public runtime::JvmObject
{
  public:
    ZAttachedArrayForForwarding(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t length() const noexcept { return read_field<uint64_t>(length_offset_); }

    uint64_t object_size() const noexcept;

    ZForwardingEntry entry(ZForwarding obj) const noexcept;

  private:
    static uint64_t align_up(uint64_t size, uint64_t alignment) noexcept;

    DECLARE_STATIC_INIT

    static inline uint64_t length_offset_;
};
} // namespace hotspot::gc::z