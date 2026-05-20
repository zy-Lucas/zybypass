#include "mark.hpp"

namespace hotspot::oops
{
void Mark::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("oopDesc");

    mark_offset_ = *type->field_offset("_mark");

    age_bits_ = *runtime::Jvm::lookup_int_constant("markWord::age_bits");
    lock_bits_ = *runtime::Jvm::lookup_int_constant("markWord::lock_bits");
    biased_lock_bits_ = *runtime::Jvm::lookup_int_constant("markWord::biased_lock_bits");
    max_hash_bits_ = *runtime::Jvm::lookup_int_constant("markWord::max_hash_bits");
    hash_bits_ = *runtime::Jvm::lookup_int_constant("markWord::hash_bits");
    lock_shift_ = *runtime::Jvm::lookup_int_constant("markWord::lock_shift");
    biased_lock_shift_ = *runtime::Jvm::lookup_int_constant("markWord::biased_lock_shift");
    age_shift_ = *runtime::Jvm::lookup_int_constant("markWord::age_shift");
    hash_shift_ = *runtime::Jvm::lookup_int_constant("markWord::hash_shift");
    lock_mask_ = *runtime::Jvm::lookup_int_constant("markWord::lock_mask");
    lock_mask_in_place_ = *runtime::Jvm::lookup_int_constant("markWord::lock_mask_in_place");
    biased_lock_mask_ = *runtime::Jvm::lookup_int_constant("markWord::biased_lock_mask");
    biased_lock_mask_in_place_ = *runtime::Jvm::lookup_int_constant("markWord::biased_lock_mask_in_place");
    biased_lock_bit_in_place_ = *runtime::Jvm::lookup_int_constant("markWord::biased_lock_bit_in_place");
    age_mask_ = *runtime::Jvm::lookup_int_constant("markWord::age_mask");
    age_mask_in_place_ = *runtime::Jvm::lookup_int_constant("markWord::age_mask_in_place");
    hash_mask_ = *runtime::Jvm::lookup_int_constant("markWord::hash_mask");
    hash_mask_in_place_ = *runtime::Jvm::lookup_int_constant("markWord::hash_mask_in_place");
    biased_lock_alignment_ = *runtime::Jvm::lookup_int_constant("markWord::biased_lock_alignment");
    locked_value_ = *runtime::Jvm::lookup_int_constant("markWord::locked_value");
    unlocked_value_ = *runtime::Jvm::lookup_int_constant("markWord::unlocked_value");
    monitor_value_ = *runtime::Jvm::lookup_int_constant("markWord::monitor_value");
    marked_value_ = *runtime::Jvm::lookup_int_constant("markWord::marked_value");
    biased_lock_pattern_ = *runtime::Jvm::lookup_int_constant("markWord::biased_lock_pattern");
    no_hash_ = *runtime::Jvm::lookup_int_constant("markWord::no_hash");
    no_hash_in_place_ = *runtime::Jvm::lookup_int_constant("markWord::no_hash_in_place");
    no_lock_in_place_ = *runtime::Jvm::lookup_int_constant("markWord::no_lock_in_place");
    max_age_ = *runtime::Jvm::lookup_int_constant("markWord::max_age");
}
} // namespace hotspot::oops