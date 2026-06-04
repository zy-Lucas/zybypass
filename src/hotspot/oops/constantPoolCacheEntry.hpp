#pragma once

#include "oops/method.hpp"

namespace hotspot::oops
{
class ConstantPoolCacheEntry
{
  public:
    ConstantPoolCacheEntry(ConstantPoolCache cp, uint32_t index) noexcept;

    int64_t indices() const noexcept { return runtime::Jvm::read<int64_t>(cp_.address() + offset_ + indices_offset_); }

    std::pair<std::string_view, runtime::JvmObjectPtr> f1() const;
    void set_f1(uint64_t f1) noexcept { runtime::Jvm::write(cp_.address() + offset_ + f1_offset_, f1); }

    int64_t f2() const noexcept { return runtime::Jvm::read<int64_t>(cp_.address() + offset_ + f2_offset_); }
    void set_f2(int64_t f2) noexcept { runtime::Jvm::write(cp_.address() + offset_ + f2_offset_, f2); }

    int64_t flags() const noexcept { return runtime::Jvm::read<int64_t>(cp_.address() + offset_ + flags_offset_); }

    bool is_volatile() const noexcept { return flags() & 1 << is_volatile_shift_; }
    bool is_final() const noexcept { return flags() & 1 << is_final_shift_; }
    bool is_forced_virtual() const noexcept { return flags() & 1 << is_forced_virtual_shift_; }
    bool is_vfinal() const noexcept { return flags() & 1 << is_vfinal_shift_; }
    bool is_method_entry() const noexcept { return !is_field_entry(); }
    bool is_field_entry() const noexcept { return flags() & 1 << is_field_entry_shift_; }

    void adjust_method_entry(Method old_method, Method new_method);
    Method interesting_method_entry() const;

  private:
    ConstantPoolCache cp_;
    uint64_t offset_;

    DECLARE_STATIC_INIT

    static inline uint64_t indices_offset_;
    static inline uint64_t f1_offset_;
    static inline uint64_t f2_offset_;
    static inline uint64_t flags_offset_;

    static inline uint64_t size_;

    static inline int32_t is_volatile_shift_;
    static inline int32_t is_final_shift_;
    static inline int32_t is_forced_virtual_shift_;
    static inline int32_t is_vfinal_shift_;
    static inline int32_t is_field_entry_shift_;
    static inline int32_t tos_state_shift_;
};
} // namespace hotspot::oops