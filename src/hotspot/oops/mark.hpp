#pragma once

#include "../runtime/jvmObject.hpp"
#include <cstdint>

namespace hotspot::oops
{
class Mark : public runtime::JvmObject
{
  public:
    Mark(uint64_t addr) : runtime::JvmObject(addr) {}

    uint64_t value() const noexcept { return read_field<uint64_t>(mark_offset); }

    bool has_bias_pattern() const noexcept { return (value() & biased_lock_mask_in_place) == biased_lock_pattern; }

    bool is_locked() const noexcept { return (value() & lock_mask_in_place) != unlocked_value; }
    bool is_unlocked() const noexcept { return (value() & biased_lock_mask_in_place) == unlocked_value; }
    bool is_marked() const noexcept { return (value() & lock_mask_in_place) == marked_value; }

    bool has_locker() const noexcept { return (value() & lock_mask_in_place) == locked_value; }
    bool has_monitor() const noexcept { return value() & monitor_value; }
    bool has_displaced_mark_helper() const noexcept { return !(value() & unlocked_value); }

    int32_t age() const noexcept { return value() >> age_shift & age_mask; }

    uint64_t hash() const noexcept { return value() >> hash_shift & hash_mask; }
    bool has_no_hash() const noexcept { return hash() == no_hash; }

    uint64_t get_size() const noexcept { return value() >> size_shift; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t mark_offset;

    static inline int64_t age_bits;
    static inline int64_t lock_bits;
    static inline int64_t biased_lock_bits;
    static inline int64_t max_hash_bits;
    static inline int64_t hash_bits;
    static inline int64_t lock_shift;
    static inline int64_t biased_lock_shift;
    static inline int64_t age_shift;
    static inline int64_t hash_shift;
    static inline int64_t lock_mask;
    static inline int64_t lock_mask_in_place;
    static inline int64_t biased_lock_mask;
    static inline int64_t biased_lock_mask_in_place;
    static inline int64_t biased_lock_bit_in_place;
    static inline int64_t age_mask;
    static inline int64_t age_mask_in_place;
    static inline int64_t hash_mask;
    static inline int64_t hash_mask_in_place;
    static inline int64_t biased_lock_alignment;
    static inline int64_t locked_value;
    static inline int64_t unlocked_value;
    static inline int64_t monitor_value;
    static inline int64_t marked_value;
    static inline int64_t biased_lock_pattern;
    static inline int64_t no_hash;
    static inline int64_t no_hash_in_place;
    static inline int64_t no_lock_in_place;
    static inline int64_t max_age;
    static inline int64_t cms_shift;
    static inline int64_t cms_mask;
    static inline int64_t size_shift;
};
} // namespace hotspot::oops