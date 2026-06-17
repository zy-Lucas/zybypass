#pragma once

#include "runtime/jvmObject.hpp"

namespace hotspot::oops
{
class DataLayout : public runtime::JvmObject
{
  public:
    DataLayout(uint64_t addr, uint64_t offset) noexcept : runtime::JvmObject(addr), offset_(offset) {}

    uint64_t dp() const noexcept { return offset_; }

    int64_t cell_at(uint32_t index) const noexcept { return read_field<int64_t>(offset_ + cell_offset(index)); }
    void set_cell_at(uint32_t index, int64_t value) noexcept { write_field(offset_ + cell_offset(index), value); }
    uint64_t address_at(uint32_t index) const noexcept { return read_field<uint64_t>(offset_ + cell_offset(index)); }

    uint8_t tag() const noexcept { return u1_at(0); }
    uint8_t flags() const noexcept { return u1_at(1); }
    uint16_t bci() const noexcept { return u2_at(2); }

    uint32_t trap_state() const noexcept { return read_field<uint32_t>(offset_ + 4); }
    void set_trap_state(uint32_t new_state) noexcept { write_field(offset_ + 4, new_state | trap_state()); }

    bool flag_at(uint8_t flag_number) const noexcept { return flags() & (0x1 << flag_number); }
    void set_flag_at(uint8_t flag_number) noexcept { set_u1_at(1, flags() | (0x1 << flag_number)); }

    static uint32_t cell_offset(uint32_t index) noexcept { return (header_size_in_cells() + index) * sizeof(void *); }

    static uint32_t header_size_in_bytes() noexcept { return header_size_in_cells() * sizeof(void *); }
    static uint32_t header_size_in_cells() noexcept { return 1; }

    static uint32_t compute_size_in_bytes(uint32_t cell_count) noexcept;

    enum
    {
        cell_size = sizeof(int64_t)
    };

    enum
    {
        no_tag,
        bit_data_tag,
        counter_data_tag,
        jump_data_tag,
        receiver_type_data_tag,
        virtual_call_data_tag,
        ret_data_tag,
        branch_data_tag,
        multi_branch_data_tag,
        arg_info_data_tag,
        call_type_data_tag,
        virtual_call_type_data_tag,
        parameters_type_data_tag,
        speculative_trap_data_tag
    };

  private:
    uint8_t u1_at(uint32_t at) const noexcept { return read_field<uint8_t>(offset_ + at); }
    void set_u1_at(uint32_t at, uint8_t u1) noexcept { write_field(offset_ + at, u1); }

    uint16_t u2_at(uint32_t at) const noexcept { return read_field<uint16_t>(offset_ + at); }
    void set_u2_at(uint32_t at, uint16_t u2) noexcept { write_field(offset_ + at, u2); }

    uint64_t offset_;
};
} // namespace hotspot::oops