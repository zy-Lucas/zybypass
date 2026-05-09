#include "vmSymbols.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
Symbol vmSymbols::symbol_at(uint32_t id) noexcept
{
    if (id < FIRST_SID || id >= SID_LIMIT)
        return 0;
    return runtime::Jvm::read<uint64_t>(symbols_address + id * sizeof(void *));
}

void vmSymbols::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Symbol");

    symbols_address = *type->get_field_offset("_vm_symbols[0]");

    FIRST_SID = *runtime::Jvm::lookup_int_constant("vmSymbols::FIRST_SID");
    SID_LIMIT = *runtime::Jvm::lookup_int_constant("vmSymbols::SID_LIMIT");
}
} // namespace hotspot::oops