#include "oopHandle.hpp"

namespace hotspot::debugger
{
uint64_t OopHandle::compressed_klass_address_at(uint64_t offset) const noexcept
{
    return runtime::Jvm::read_compressed_klass_address_value(address() + offset);
}

void OopHandle::write_compressed_klass_address_at(uint64_t offset, uint64_t addr) const noexcept
{
    runtime::Jvm::write_compressed_klass_address_value(address() + offset, addr);
}

OopHandle OopHandle::compressed_oop_handle_at(uint64_t offset) const noexcept
{
    return runtime::Jvm::read_compressed_oop_address_value(address() + offset);
}

void OopHandle::write_compressed_oop_handle_at(uint64_t offset, OopHandle handle) const noexcept
{
    runtime::Jvm::write_compressed_oop_address_value(address() + offset, handle.address());
}
} // namespace hotspot::debugger