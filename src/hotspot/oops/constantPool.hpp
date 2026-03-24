#pragma once

#include "../runtime/jvmObject.hpp"
#include "symbol.hpp"

namespace hotspot::oops
{
class ConstantPool : public runtime::JvmObject<ConstantPool>
{
  public:
    ConstantPool(uint64_t addr);

    int32_t get_length() const noexcept { return read_field<int32_t>(length_offset); }
    uint16_t get_major() const noexcept { return read_field<uint16_t>(major_version_offset); }
    uint16_t get_minor() const noexcept { return read_field<uint16_t>(minor_version_offset); }
    
    uint64_t index_offset(uint32_t index) { return (index * element_size) + header_size; }
    uint64_t get_address_at_raw(uint32_t index) { return read_field<uint64_t>(index_offset(index)); }

    Symbol get_symbol_at(uint32_t index) { return {get_address_at_raw(index)}; }

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

    static inline uint64_t INDY_BSM_OFFSET;
    static inline uint64_t INDY_ARGC_OFFSET;
    static inline uint64_t INDY_ARGV_OFFSET;

    static void initialize();
};
} // namespace hotspot::oops