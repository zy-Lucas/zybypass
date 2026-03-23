#include "symbol.hpp"

namespace hotspot::oops
{
Symbol::Symbol(uint64_t addr) : runtime::JvmObject<Symbol>(addr) {}

void Symbol::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Symbol");

    length_offset = type->get_field("_length")->get_offset();
    base_offset = type->get_field("_base")->get_offset();
    id_hash_and_refcount_offset = type->get_field("_hash_and_refcount")->get_offset();
}
} // namespace hotspot::oops