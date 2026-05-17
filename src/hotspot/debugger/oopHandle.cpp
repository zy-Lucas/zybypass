#include "oopHandle.hpp"

namespace hotspot::debugger
{
uint64_t OopHandle::get_comp_klass_address_at(uint64_t offset) const noexcept
{
    return runtime::Jvm::read_comp_klass_address_value(address() + offset);
}
} // namespace hotspot::debugger