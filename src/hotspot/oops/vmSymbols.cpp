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
    utils::FieldResolver r{"Symbol"};

    r.field_offset("_vm_symbols[0]", symbols_address_);

    utils::constants::int_const("vmSymbols::FIRST_SID", first_sid_);
    utils::constants::int_const("vmSymbols::SID_LIMIT", sid_limit_);
}
} // namespace hotspot::oops