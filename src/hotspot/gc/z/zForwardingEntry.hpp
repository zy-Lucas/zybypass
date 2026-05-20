#pragma once

#include "../../runtime/jvmObject.hpp"

namespace hotspot::gc::z
{
class ZForwardingEntry : public runtime::JvmObject
{
  public:
    ZForwardingEntry(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t entry() const noexcept { return read_field<uint64_t>(entry_offset_); }

    bool populated() const noexcept { return field_populated_decode(entry()); }
    uint64_t to_offset() const noexcept { return field_to_offset_decode(entry()); }
    uint64_t from_index() const noexcept { return field_from_index_decode(entry()); }

    static uint64_t size() noexcept { return type_->size(); }

  private:
    static bool field_populated_decode(uint64_t value) noexcept;
    static uint64_t field_to_offset_decode(uint64_t value) noexcept;
    static uint64_t field_from_index_decode(uint64_t value) noexcept;

    DECLARE_STATIC_INIT

    static inline types::Type *type_;

    static inline uint64_t entry_offset_;
};
} // namespace hotspot::gc::z