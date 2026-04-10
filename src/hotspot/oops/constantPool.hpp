#pragma once

#include "metaData.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
struct CPSlot
{
    uint64_t ptr;
    Symbol get_symbol() { return ptr & (~1); }
};

struct CPKlassSlot
{
    uint16_t name_index;
    uint16_t resolved_klass_index;
};

class ConstantPool : public MetaData
{
  public:
    ConstantPool(uint64_t addr);

    int32_t get_length() const noexcept { return read_field<int32_t>(length_offset); }
    uint16_t get_major() const noexcept { return read_field<uint16_t>(major_version_offset); }
    uint16_t get_minor() const noexcept { return read_field<uint16_t>(minor_version_offset); }

    Symbol get_source_file_name() const noexcept
    {
        return get_symbol_at(read_field<uint16_t>(source_file_name_index_offset));
    }
    Symbol get_generic_signature() const noexcept
    {
        return get_symbol_at(read_field<uint16_t>(generic_signature_index_offset));
    }

    uint64_t index_offset(uint32_t index) const noexcept { return (index * element_size) + header_size; }

    uint64_t get_address_at_raw(uint32_t index) const noexcept { return read_field<uint64_t>(index_offset(index)); }
    CPSlot get_slot_at(uint32_t index) const noexcept { return {get_address_at_raw(index)}; }
    CPKlassSlot get_klass_slot_at(uint32_t index) const noexcept;

    Symbol get_symbol_at(uint32_t index) const noexcept { return get_address_at_raw(index); }
    int32_t get_int_at(uint32_t index) const noexcept { return read_field<int32_t>(index_offset(index)); }
    int64_t get_long_at(uint32_t index) const noexcept;
    float get_float_at(uint32_t index) const noexcept { return read_field<float>(index_offset(index)); }
    double get_double_at(uint32_t index) const noexcept { return std::bit_cast<double>(get_long_at(index)); }

    void set_address_at_raw(uint32_t index, uint64_t addr) noexcept { write_field(index_offset(index), addr); }

    void set_symbol_at(uint32_t index, uint64_t addr) noexcept { set_address_at_raw(index, addr); }
    void set_int_at(uint32_t index, int32_t value) noexcept { write_field(index_offset(index), value); }

    uint64_t get_size() const noexcept { return align_size(get_length() + header_size); }

    static uint64_t get_header_size() noexcept { return header_size; }
    static uint64_t get_element_size() noexcept { return element_size; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t tags_offset;
    static inline uint64_t operands_offset;
    static inline uint64_t cache_offset;
    static inline uint64_t pool_holder_offset;
    static inline uint64_t length_offset;
    static inline uint64_t resolved_klasses_offset;
    static inline uint64_t major_version_offset;
    static inline uint64_t minor_version_offset;
    static inline uint64_t source_file_name_index_offset;
    static inline uint64_t generic_signature_index_offset;

    static inline uint64_t header_size;
    static inline uint64_t element_size;

    static inline int32_t INDY_BSM_OFFSET;
    static inline int32_t INDY_ARGC_OFFSET;
    static inline int32_t INDY_ARGV_OFFSET;
};
} // namespace hotspot::oops