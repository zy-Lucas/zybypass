#pragma once

#include "runtime/jvmObject.hpp"
#include "zForwardingTable.hpp"
#include "zRelocate.hpp"

namespace hotspot::gc::z
{
class ZHeap : public runtime::JvmObject
{
  public:
    ZHeap(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    ZForwardingTable forwarding_table() const noexcept { return address() + forwarding_table_offset_; }

    ZRelocate relocate() const noexcept { return address() + relocate_offset_; }

    uint64_t relocate_object(uint64_t addr) const noexcept;
    uint64_t remap_object(uint64_t o) const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t page_allocator_offset_;
    static inline uint64_t page_table_offset_;
    static inline uint64_t forwarding_table_offset_;
    static inline uint64_t relocate_offset_;
};
} // namespace hotspot::gc::z