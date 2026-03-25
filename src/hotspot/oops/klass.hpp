#pragma once

#include "metaData.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
class Klass : public MetaData
{
  public:
    Klass(uint64_t addr);

    uint32_t get_class_status() { return 0; }

    bool is_array_klass() { return false; }

    Klass get_super() const noexcept { return {read_field<uint64_t>(super_offset)}; }
    int32_t get_layout_helper() const noexcept { return read_field<int32_t>(layout_helper_offset); }
    Symbol get_name() const noexcept { return {read_field<uint64_t>(name_offset)}; }
    int32_t get_access_flags() const noexcept { return read_field<int32_t>(access_flags_offset); }
    Klass get_subklass() const noexcept { return {read_field<uint64_t>(subklass_offset)}; }
    Klass get_next_sibling() const noexcept { return {read_field<uint64_t>(next_sibling_offset)}; }
    Klass get_next_link() const noexcept { return {read_field<uint64_t>(next_link_offset)}; }
    uint64_t get_vtable_len() const noexcept { return read_field<uint64_t>(vtable_len_offset); }

    bool is_subclass_of(const Klass &k) const noexcept;

  private:
    static inline std::once_flag init_flag_;

    static inline uint64_t java_mirror_offset;
    static inline uint64_t super_offset;
    static inline uint64_t layout_helper_offset;
    static inline uint64_t name_offset;
    static inline uint64_t access_flags_offset;
    static inline uint64_t subklass_offset;
    static inline uint64_t next_sibling_offset;
    static inline uint64_t next_link_offset;
    static inline uint64_t vtable_len_offset;
    static inline uint64_t class_loader_data_offset;

    static inline std::optional<uint64_t> trace_id_offset;

    static inline int32_t LH_INSTANCE_SLOW_PATH_BIT;
    static inline int32_t LH_LOG2_ELEMENT_SIZE_SHIFT;
    static inline int32_t LH_LOG2_ELEMENT_SIZE_MASK;
    static inline int32_t LH_ELEMENT_TYPE_SHIFT;
    static inline int32_t LH_HEADER_SIZE_SHIFT;
    static inline int32_t LH_HEADER_SIZE_MASK;
    static inline int32_t LH_ARRAY_TAG_SHIFT;
    static inline int32_t LH_ARRAY_TAG_TYPE_VALUE;
    static inline int32_t LH_ARRAY_TAG_OBJ_VALUE;

    static void initialize();
};
} // namespace hotspot::oops