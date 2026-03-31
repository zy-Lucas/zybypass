#include "symbol.hpp"

namespace hotspot::oops
{
Symbol::Symbol(uint64_t addr) : runtime::JvmObject<Symbol>(addr) {}

void Symbol::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Symbol");

    length_offset = *type->get_field_offset("_length");
    base_offset = *type->get_field_offset("_body");
    id_hash_and_refcount_offset = *type->get_field_offset("_hash_and_refcount");
}
} // namespace hotspot::oops