#pragma once

#include "oops/mark.hpp"
#include "runtime/accessFlag.hpp"
#include "metaData.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
class klassVtable;
class Instance;

class Klass : public MetaData
{
  public:
    Klass(uint64_t addr) noexcept : MetaData(addr) {}

    Instance java_mirror() const;
    Klass super() const noexcept { return read_field<uint64_t>(super_offset_); }
    int32_t layout_helper() const noexcept { return read_field<int32_t>(layout_helper_offset_); }
    Symbol name() const noexcept { return read_field<uint64_t>(name_offset_); }
    int32_t access_flags() const noexcept { return read_field<int32_t>(access_flags_offset_); }
    runtime::AccessFlags access_flags_obj() const noexcept { return access_flags(); }
    Klass subklass() const noexcept { return read_field<uint64_t>(subklass_offset_); }
    Klass next_sibling() const noexcept { return read_field<uint64_t>(next_sibling_offset_); }
    Klass next_link() const noexcept { return read_field<uint64_t>(next_link_offset_); }
    int32_t vtable_length() const noexcept { return read_field<int32_t>(vtable_len_offset_); }
    Mark prototype_header() const noexcept { return address() + prototype_header_offset_; }

    uint64_t trace_id() const noexcept { return trace_id_offset_ ? read_field<uint64_t>(*trace_id_offset_) : 0; }

    bool is_subclass_of(const Klass &k) const noexcept;

    bool is_instance_klass() const noexcept { return is_instance(layout_helper()); }
    bool is_array_klass() const noexcept { return is_array(layout_helper()); }
    bool is_typeArray_klass() const noexcept { return is_typeArray(layout_helper()); }
    bool is_objArray_klass() const noexcept { return is_objArray(layout_helper()); }

    bool is_public() const noexcept { return access_flags_obj().is_public(); }
    bool is_final() const noexcept { return access_flags_obj().is_final(); }
    bool is_interface() const noexcept { return access_flags_obj().is_interface(); }
    bool is_abstract() const noexcept { return access_flags_obj().is_abstract(); }
    bool is_super() const noexcept { return access_flags_obj().is_super(); }
    bool is_synthetic() const noexcept { return access_flags_obj().is_synthetic(); }
    bool has_finalizer() const noexcept { return access_flags_obj().has_finalizer(); }
    bool is_cloneable() const noexcept { return access_flags_obj().is_cloneable(); }
    bool has_vanilla_constructor() const noexcept { return access_flags_obj().has_vanilla_constructor(); }
    bool has_miranda_methods() const noexcept { return access_flags_obj().has_miranda_methods(); }

    uint64_t start_of_vtable() const noexcept;

    klassVtable vtable() const noexcept;

    static bool is_instance(int32_t lh) noexcept { return lh > lh_neutral_value_; }
    static bool is_array(int32_t lh) noexcept { return lh < lh_neutral_value_; }
    static bool is_typeArray(int32_t lh) noexcept { return lh >= (lh_array_tag_type_value_ << lh_array_tag_shift_); }
    static bool is_objArray(int32_t lh) noexcept { return lh < (lh_array_tag_type_value_ << lh_array_tag_shift_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t java_mirror_offset_;
    static inline uint64_t super_offset_;
    static inline uint64_t layout_helper_offset_;
    static inline uint64_t name_offset_;
    static inline uint64_t access_flags_offset_;
    static inline uint64_t subklass_offset_;
    static inline uint64_t next_sibling_offset_;
    static inline uint64_t next_link_offset_;
    static inline uint64_t class_loader_data_offset_;
    static inline uint64_t vtable_len_offset_;
    static inline uint64_t prototype_header_offset_;

    static inline std::optional<uint64_t> trace_id_offset_;

  public:
    static inline int32_t lh_neutral_value_;
    static inline int32_t lh_instance_slow_path_bit_;
    static inline int32_t lh_log2_element_size_shift_;
    static inline int32_t lh_log2_element_size_mask_;
    static inline int32_t lh_element_type_shift_;
    static inline int32_t lh_header_size_shift_;
    static inline int32_t lh_header_size_mask_;
    static inline int32_t lh_array_tag_shift_;
    static inline int32_t lh_array_tag_type_value_;
    static inline int32_t lh_array_tag_obj_value_;
};
} // namespace hotspot::oops