#pragma once

#include "../utilities/array.hpp"
#include "constantPoolCache.hpp"
#include "metaData.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
class InstanceKlass;

struct CPSlot
{
    Symbol symbol() const noexcept { return ptr_ & (~1); }

    uint64_t ptr_;
};

struct CPKlassSlot
{
    uint16_t name_index_;
    uint16_t resolved_klass_index_;
};

class ConstantPool : public MetaData
{
  public:
    ConstantPool(uint64_t addr) noexcept : MetaData(addr) {};

    utilities::U1Array tags() const noexcept { return read_field<uint64_t>(tags_offset_); }
    void set_tags(uint64_t addr) noexcept { write_field(tags_offset_, addr); }

    ConstantPoolCache cache() const noexcept { return read_field<uint64_t>(cache_offset_); }
    void set_cache(uint64_t addr) noexcept { write_field(cache_offset_, addr); }

    InstanceKlass pool_holder() const noexcept;
    void set_pool_holder(uint64_t addr) noexcept { write_field(pool_holder_offset_, addr); }

    utilities::U2Array operands() const noexcept { return read_field<uint64_t>(operands_offset_); }
    void set_operands(uint64_t addr) noexcept { write_field(operands_offset_, addr); }

    utilities::KlassArray resolved_klasses() const noexcept;
    void set_resolved_klasses(uint64_t addr) noexcept { write_field(resolved_klasses_offset_, addr); }

    uint16_t major() const noexcept { return read_field<uint16_t>(major_version_offset_); }
    void set_major(uint16_t major) noexcept { write_field(major_version_offset_, major); }

    uint16_t minor() const noexcept { return read_field<uint16_t>(minor_version_offset_); }
    void set_minor(uint16_t minor) noexcept { write_field(minor_version_offset_, minor); }

    Symbol generic_signature() const noexcept;
    Symbol source_file_name() const noexcept;
    
    int32_t length() const noexcept { return read_field<int32_t>(length_offset_); }
    void set_length(int32_t length) noexcept { write_field(length_offset_, length); }

    uint64_t index_offset(uint32_t index) const noexcept { return (index * element_size_) + header_size_; }

    uint64_t address_at_raw(uint32_t index) const noexcept { return read_field<uint64_t>(index_offset(index)); }
    void set_address_at_raw(uint32_t index, uint64_t addr) noexcept { write_field(index_offset(index), addr); }

    void set_generic_signature_index(uint16_t index) noexcept { write_field(generic_signature_index_offset_, index); }
    void set_source_file_name_index(uint16_t index) noexcept { write_field(source_file_name_index_offset_, index); }

    CPSlot slot_at(uint32_t index) const noexcept { return {address_at_raw(index)}; }
    CPKlassSlot get_klass_slot_at(uint32_t index) const noexcept;

    Symbol symbol_at(uint32_t index) const noexcept { return address_at_raw(index); }
    void set_symbol_at(uint32_t index, uint64_t addr) noexcept { set_address_at_raw(index, addr); }

    int32_t int_at(uint32_t index) const noexcept { return read_field<int32_t>(index_offset(index)); }
    void set_int_at(uint32_t index, int32_t value) noexcept { write_field(index_offset(index), value); }

    int64_t long_at(uint32_t index) const noexcept;
    float float_at(uint32_t index) const noexcept { return read_field<float>(index_offset(index)); }
    double double_at(uint32_t index) const noexcept { return std::bit_cast<double>(long_at(index)); }

    uint64_t size() const noexcept { return align_size(length() + header_size_); }

    static uint64_t header_size() noexcept { return header_size_; }
    static uint64_t element_size() noexcept { return element_size_; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t tags_offset_;
    static inline uint64_t cache_offset_;
    static inline uint64_t pool_holder_offset_;
    static inline uint64_t operands_offset_;
    static inline uint64_t resolved_klasses_offset_;
    static inline uint64_t major_version_offset_;
    static inline uint64_t minor_version_offset_;
    static inline uint64_t generic_signature_index_offset_;
    static inline uint64_t source_file_name_index_offset_;
    static inline uint64_t length_offset_;

    static inline uint64_t header_size_;
    static inline uint64_t element_size_;

    static inline int32_t indy_bsm_offset_;
    static inline int32_t indy_argc_offset_;
    static inline int32_t indy_argv_offset_;
};
} // namespace hotspot::oops