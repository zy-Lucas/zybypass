#include "symbol.hpp"

namespace hotspot::oops
{
void Symbol::initialize()
{
    utils::FieldResolver r{"Symbol"};

    r.field_offset("_length", length_offset_);
    r.field_offset("_body", base_offset_);
    r.field_offset("_hash_and_refcount", id_hash_and_refcount_offset_);
}
} // namespace hotspot::oops