#pragma once

#include "../../runtime/jvmObject.hpp"
#include "zAttachedArrayForForwarding.hpp"
#include "zVirtualMemory.hpp"

namespace hotspot::gc::z
{
class ZForwarding : public runtime::JvmObject
{
  public:
    ZForwarding(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t start() const noexcept { return virtual_memory().start(); }

    uint64_t object_alignment_shift() const noexcept { return read_field<uint64_t>(object_alignment_shift_offset_); }

    bool retain_page() const noexcept { return read_field<int32_t>(ref_count_offset_) > 0; }

    ZForwardingEntry find(uint64_t from_index) noexcept;

    static uint64_t size() noexcept { return type_->size(); }

  private:
    ZVirtualMemory virtual_memory() const noexcept { return address() + virtual_offset_; }

    ZAttachedArrayForForwarding entries() const noexcept { return address() + entries_offset_; }

    ZForwardingEntry at(uint64_t cursor) const noexcept;

    class ZForwardEntryIterator
    {
      public:
        ZForwardEntryIterator(uint64_t from_index, ZForwarding *forwarding) noexcept;

        bool has_next() const noexcept { return next_entry_.populated(); }

        ZForwardingEntry next() noexcept;
        ZForwardingEntry peak() const noexcept { return next_entry_; }

      private:
        uint64_t mask_;
        uint64_t cursor_;

        ZForwarding *forwarding_;
        ZForwardingEntry next_entry_;
    };

    static uint32_t uint32_to_uint32(uint32_t key) noexcept;

    DECLARE_STATIC_INIT

    static inline types::Type *type_;

    static inline uint64_t virtual_offset_;
    static inline uint64_t entries_offset_;
    static inline uint64_t object_alignment_shift_offset_;
    static inline uint64_t ref_count_offset_;
};
} // namespace hotspot::gc::z