#include "mark.hpp"

namespace hotspot::oops
{
void Mark::initialize()
{
    utils::FieldResolver r{"oopDesc"};

    r.field_offset("_mark", mark_offset_);

    utils::constants::long_const("markWord::age_bits", age_bits_);
    utils::constants::long_const("markWord::lock_bits", lock_bits_);
    utils::constants::long_const("markWord::biased_lock_bits", biased_lock_bits_);
    utils::constants::long_const("markWord::max_hash_bits", max_hash_bits_);
    utils::constants::long_const("markWord::hash_bits", hash_bits_);
    utils::constants::long_const("markWord::lock_shift", lock_shift_);
    utils::constants::long_const("markWord::biased_lock_shift", biased_lock_shift_);
    utils::constants::long_const("markWord::age_shift", age_shift_);
    utils::constants::long_const("markWord::hash_shift", hash_shift_);
    utils::constants::long_const("markWord::lock_mask", lock_mask_);
    utils::constants::long_const("markWord::lock_mask_in_place", lock_mask_in_place_);
    utils::constants::long_const("markWord::biased_lock_mask", biased_lock_mask_);
    utils::constants::long_const("markWord::biased_lock_mask_in_place", biased_lock_mask_in_place_);
    utils::constants::long_const("markWord::biased_lock_bit_in_place", biased_lock_bit_in_place_);
    utils::constants::long_const("markWord::age_mask", age_mask_);
    utils::constants::long_const("markWord::age_mask_in_place", age_mask_in_place_);
    utils::constants::long_const("markWord::hash_mask", hash_mask_);
    utils::constants::long_const("markWord::hash_mask_in_place", hash_mask_in_place_);
    utils::constants::long_const("markWord::biased_lock_alignment", biased_lock_alignment_);
    utils::constants::long_const("markWord::locked_value", locked_value_);
    utils::constants::long_const("markWord::unlocked_value", unlocked_value_);
    utils::constants::long_const("markWord::monitor_value", monitor_value_);
    utils::constants::long_const("markWord::marked_value", marked_value_);
    utils::constants::long_const("markWord::biased_lock_pattern", biased_lock_pattern_);
    utils::constants::long_const("markWord::no_hash", no_hash_);
    utils::constants::long_const("markWord::no_hash_in_place", no_hash_in_place_);
    utils::constants::long_const("markWord::no_lock_in_place", no_lock_in_place_);
    utils::constants::long_const("markWord::max_age", max_age_);
}
} // namespace hotspot::oops