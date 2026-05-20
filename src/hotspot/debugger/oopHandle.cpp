#include "oopHandle.hpp"

namespace hotspot::debugger
{
uint64_t OopHandle::compressed_klass_address_at(uint64_t offset) const noexcept
{
    return runtime::Jvm::read_compressed_klass_address_value(address() + offset);
}
} // namespace hotspot::debugger