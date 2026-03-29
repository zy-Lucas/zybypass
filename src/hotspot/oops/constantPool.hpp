#pragma once

#include "../runtime/jvmObject.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
struct CPSlot
{
    uint64_t ptr;
    Symbol get_symbol() { return ptr & (~1); }
};

class ConstantPool : public runtime::JvmObject<ConstantPool>
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
    Symbol get_symbol_at(uint32_t index) const noexcept { return index ? get_address_at_raw(index) : 0; }

    int32_t get_int_at(uint32_t index) const noexcept { return read_field<int32_t>(index_offset(index)); }
    int64_t get_long_at(uint32_t index) const noexcept
    {
        return runtime::Jvm::build_long_from_intsPD(read_field<int32_t>(index_offset(index)),
                                                    read_field<int32_t>(index_offset(index + 1)));
    }
    float get_float_at(uint32_t index) const noexcept { return read_field<float>(index_offset(index)); }
    double get_double_at(uint32_t index) const noexcept { return std::bit_cast<double>(get_long_at(index)); }

  private:
    friend class runtime::JvmObject<ConstantPool>;

    static inline std::once_flag init_flag_;

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

    static void initialize();
};
} // namespace hotspot::oops