#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::debugger
{
struct OopHandle : public runtime::JvmObject
{
    OopHandle(uint64_t addr) : runtime::JvmObject(addr) {}

    uint64_t get_comp_klass_address_at(uint64_t offset) const noexcept;

    OopHandle get_oop_handle_at(uint64_t offset) const noexcept { return read_field<uint64_t>(offset); }
};
} // namespace hotspot::debugger