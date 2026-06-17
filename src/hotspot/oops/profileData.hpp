#pragma once

#include "oops/dataLayout.hpp"

namespace hotspot::oops
{
class Method;

class ProfileData
{
  public:
    ProfileData(DataLayout layout) noexcept : data_(layout) {}

    virtual int32_t cell_count() const noexcept { return -1; }
    uint32_t size_in_bytes() const noexcept { return DataLayout::compute_size_in_bytes(cell_count()); }

    uint16_t bci() const noexcept { return data_.bci(); }
    uint64_t dp() const noexcept { return data_.address(); }

    bool flag_at(uint8_t flag_number) const noexcept { return data_.flag_at(flag_number); }
    void set_flag_at(uint8_t flag_number) noexcept { data_.set_flag_at(flag_number); }

    uint32_t trap_state() const noexcept { return data_.trap_state(); }
    void set_trap_state(uint32_t new_state) noexcept { data_.set_trap_state(new_state); }

    int64_t cell_at(int32_t index) const noexcept { return data_.cell_at(index); }
    void set_cell_at(int32_t index, int64_t value) noexcept { data_.set_cell_at(index, value); }

  protected:
    enum
    {
        cell_size = DataLayout::cell_size
    };

  private:
    DataLayout data_;
};

class BitData : public ProfileData
{
    friend class VMStructs;
    friend class JVMCIVMStructs;

  protected:
    enum
    {
        null_seen_flag = 0
    };

    enum
    {
        bit_cell_count = 0
    };

  public:
    BitData(DataLayout layout) noexcept : ProfileData(layout) {}

    virtual int cell_count() const noexcept { return static_cell_count(); }

    bool null_seen() { return flag_at(null_seen_flag); }
    void set_null_seen() { set_flag_at(null_seen_flag); }

    static int static_cell_count() { return bit_cell_count; }
};

class TypeEntries
{
  public:
    enum
    {
        null_seen = 1,
        type_mask = ~null_seen,
        type_unknown = 2,
        status_bits = null_seen | type_unknown,
        type_klass_mask = ~status_bits
    };

    static bool was_null_seen(int64_t v) noexcept { return (v & null_seen) != 0; }
    static bool is_type_unknown(int64_t v) noexcept { return (v & type_unknown) != 0; }
    static bool is_type_none(int64_t v) noexcept { return (v & type_mask) == 0; }
    static int64_t klass_part(int64_t v) noexcept { return v & type_klass_mask; }
    static int64_t with_status(int64_t k, int64_t in) noexcept { return k | (in & status_bits); }

  protected:
    TypeEntries(ProfileData *pd, int32_t base_off) noexcept : pd_(pd), base_off_(base_off) {}

    int64_t intptr_at(int32_t index) const noexcept { return pd_->cell_at(base_off_ + index); }
    void set_intptr_at(int32_t index, int64_t value) noexcept { pd_->set_cell_at(base_off_ + index, value); }

    ProfileData *pd_;
    int32_t base_off_;
};

class TypeStackSlotEntries : public TypeEntries
{
  public:
    TypeStackSlotEntries(ProfileData *pd, int32_t base_off, int32_t nb_entries) noexcept;

    int32_t number_of_entries() const noexcept { return number_of_entries_; }

    uint32_t stack_slot(int32_t i) const noexcept { return pd_->cell_at(stack_slot_offset(i)); }
    void set_stack_slot(int32_t i, uint32_t num) noexcept { pd_->set_cell_at(stack_slot_offset(i), num); }

    int64_t type(int32_t i) const noexcept { return pd_->cell_at(type_offset_in_cells(i)); }
    void set_type(int32_t i, int64_t k) noexcept { pd_->set_cell_at(type_offset_in_cells(i), k); }

    static int32_t stack_slot_local_offset(int32_t i) noexcept { return i * per_arg_cell_count + stack_slot_entry; }
    static int32_t type_local_offset(int32_t i) noexcept { return i * per_arg_cell_count + type_entry; }
    static int32_t per_arg_count() noexcept { return per_arg_cell_count; }

  private:
    int32_t stack_slot_offset(int32_t i) const noexcept { return base_off_ + stack_slot_local_offset(i); }
    int32_t type_offset_in_cells(int32_t i) const noexcept { return base_off_ + type_local_offset(i); }

    enum
    {
        stack_slot_entry,
        type_entry,
        per_arg_cell_count
    };

    int32_t number_of_entries_;
};

class ArrayData : public ProfileData
{
  public:
    ArrayData(DataLayout layout) noexcept : ProfileData(layout) {}

    int32_t array_len() const noexcept { return cell_at(array_len_off_set); }
    int32_t cell_count() const noexcept override { return array_len() + 1; }

  protected:
    int64_t array_int_at(int32_t index) const noexcept { return cell_at(array_start_off_set + index); }
    void array_set_int_at(int32_t index, int32_t value) noexcept { set_cell_at(array_start_off_set + index, value); }

    enum
    {
        array_len_off_set,
        array_start_off_set
    };
};

class ParametersTypeData : public ArrayData
{
  public:
    ParametersTypeData(DataLayout layout) noexcept;

    int32_t number_of_parameters() const noexcept { return array_len() / TypeStackSlotEntries::per_arg_count(); }

    uint32_t stack_slot(int32_t i) const noexcept { return parameters_.stack_slot(i); }
    int64_t type(int32_t i) const noexcept { return parameters_.type(i); }

  private:
    TypeStackSlotEntries parameters_;
};

class SpeculativeTrapData : public ProfileData
{
  protected:
    enum
    {
        speculative_trap_method,
        speculative_trap_cell_count
    };

  public:
    SpeculativeTrapData(DataLayout layout) noexcept : ProfileData(layout) {}

    int cell_count() const noexcept override { return static_cell_count(); }

    Method method() const noexcept;
    void set_method(Method m) noexcept;

    static int static_cell_count() noexcept { return speculative_trap_cell_count; }
};
} // namespace hotspot::oops