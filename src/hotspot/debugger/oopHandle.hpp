#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::debugger
{
struct OopHandle : public runtime::JvmObject
{
    OopHandle(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t compressed_klass_address_at(uint64_t offset) const noexcept;

    OopHandle oop_handle_at(uint64_t offset) const noexcept { return read_field<uint64_t>(offset); }
    OopHandle compressed_oop_handle_at(uint64_t offset) const noexcept;
};
} // namespace hotspot::debugger