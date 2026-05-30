#pragma once

#include "../runtime/jvmObject.hpp"
#include <cstdint>

namespace hotspot::oops
{
class Mark : public runtime::JvmObject
{
  public:
    Mark(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t value() const noexcept { return read_field<uint64_t>(mark_offset_); }

    bool has_bias_pattern() const noexcept { return (value() & biased_lock_mask_in_place_) == biased_lock_pattern_; }

    bool is_locked() const noexcept { return (value() & lock_mask_in_place_) != unlocked_value_; }
    bool is_unlocked() const noexcept { return (value() & biased_lock_mask_in_place_) == unlocked_value_; }
    bool is_marked() const noexcept { return (value() & lock_mask_in_place_) == marked_value_; }

    bool has_locker() const noexcept { return (value() & lock_mask_in_place_) == locked_value_; }
    bool has_monitor() const noexcept { return value() & monitor_value_; }
    bool has_displaced_mark_helper() const noexcept { return !(value() & unlocked_value_); }

    int32_t age() const noexcept { return (value() >> age_shift_) & age_mask_; }

    uint64_t hash() const noexcept { return (value() >> hash_shift_) & hash_mask_; }
    bool has_no_hash() const noexcept { return hash() == no_hash_; }

    uint64_t get_size() const noexcept { return value() >> size_shift_; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t mark_offset_;

    static inline int64_t age_bits_;
    static inline int64_t lock_bits_;
    static inline int64_t biased_lock_bits_;
    static inline int64_t max_hash_bits_;
    static inline int64_t hash_bits_;
    static inline int64_t lock_shift_;
    static inline int64_t biased_lock_shift_;
    static inline int64_t age_shift_;
    static inline int64_t hash_shift_;
    static inline int64_t lock_mask_;
    static inline int64_t lock_mask_in_place_;
    static inline int64_t biased_lock_mask_;
    static inline int64_t biased_lock_mask_in_place_;
    static inline int64_t biased_lock_bit_in_place_;
    static inline int64_t age_mask_;
    static inline int64_t age_mask_in_place_;
    static inline int64_t hash_mask_;
    static inline int64_t hash_mask_in_place_;
    static inline int64_t biased_lock_alignment_;
    static inline int64_t locked_value_;
    static inline int64_t unlocked_value_;
    static inline int64_t monitor_value_;
    static inline int64_t marked_value_;
    static inline int64_t biased_lock_pattern_;
    static inline int64_t no_hash_;
    static inline int64_t no_hash_in_place_;
    static inline int64_t no_lock_in_place_;
    static inline int64_t max_age_;
    static inline int64_t cms_shift_;
    static inline int64_t cms_mask_;
    static inline int64_t size_shift_;
};
} // namespace hotspot::oops