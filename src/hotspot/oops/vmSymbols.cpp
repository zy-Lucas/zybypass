#include "vmSymbols.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
Symbol vmSymbols::symbol_at(uint32_t id) noexcept
{
    if (id < first_sid_ || id >= sid_limit_)
        return 0;
    return runtime::Jvm::read<uint64_t>(symbols_address_ + id * sizeof(void *));
}

void vmSymbols::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Symbol");

    symbols_address_ = *type->field_offset("_vm_symbols[0]");

    first_sid_ = *runtime::Jvm::lookup_int_constant("vmSymbols::FIRST_SID");
    sid_limit_ = *runtime::Jvm::lookup_int_constant("vmSymbols::SID_LIMIT");
}
} // namespace hotspot::oops