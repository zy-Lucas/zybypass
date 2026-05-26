#pragma once

#include "../../debugger/oopHandle.hpp"
#include "../../memory/memRegion.hpp"
#include "../../runtime/jvmObject.hpp"
#include "collectedHeapName.hpp"

namespace hotspot::gc::shared
{
class CollectedHeap : public runtime::JvmObject
{
  public:
    CollectedHeap(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    virtual ~CollectedHeap() = default;

    uint64_t start() const noexcept { return reserved_region().start(); }

    virtual uint64_t capacity() const noexcept { return 0; }
    virtual uint64_t used() const noexcept { return 0; }

    memory::MemRegion reserved_region() const noexcept { return address() + reserved_offset_; }

    virtual bool is_in(uint64_t a) const noexcept { return is_in_reserved(a); }
    virtual bool is_in_reserved(uint64_t a) const noexcept { return reserved_region().contains(a); }

    virtual CollectedHeapName kind() const noexcept;

    virtual debugger::OopHandle oop_load_at(debugger::OopHandle handle, uint64_t offset) const;
    virtual debugger::OopHandle oop_load_in_native(uint64_t addr) const;

    virtual void oop_store_at(debugger::OopHandle obj, uint64_t offset, debugger::OopHandle value) const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t reserved_offset_;
};
} // namespace hotspot::gc::shared