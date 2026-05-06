#pragma once

#include "../../runtime/jvmObject.hpp"
#include "zForwardingTable.hpp"
#include "zRelocate.hpp"

namespace hotspot::gc::z
{
class ZHeap : public runtime::JvmObject
{
  public:
    ZHeap(uint64_t addr) : runtime::JvmObject(addr) {}

    ZForwardingTable forwardingTable() const noexcept { return address() + forwarding_table_offset; }

    ZRelocate relocate() const noexcept { return address() + relocate_offset; }

    uint64_t relocate_object(uint64_t addr) const noexcept;
    uint64_t remap_object(uint64_t o) const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t page_allocator_offset;
    static inline uint64_t page_table_offset;
    static inline uint64_t forwarding_table_offset;
    static inline uint64_t relocate_offset;
};
} // namespace hotspot::gc::z