#pragma once

#include "oops/metaData.hpp"
#include "oops/profileData.hpp"

namespace hotspot::oops
{
class MethodData : public MetaData
{
  public:
    MethodData(uint64_t addr) noexcept : MetaData(addr) {}

    void clean_extra_data();

    int32_t size_in_bytes() const noexcept { return read_field<int32_t>(size_offset_); }
    int32_t data_size() const noexcept { return read_field<int32_t>(data_size_offset_); }

    int32_t parameters_size_in_bytes() const noexcept;

    DataLayout limit_data_position() const noexcept { return {address(), data_offset_ + data_size()}; }

    DataLayout extra_data_base() const noexcept { return limit_data_position(); }
    DataLayout extra_data_limit() const noexcept { return {address(), (uint64_t)size_in_bytes()}; }

    DataLayout args_data_limit() const { return {address(), uint64_t(size_in_bytes() - parameters_size_in_bytes())}; }

    ParametersTypeData parameters_type_data() const noexcept;

    int32_t parameters_type_data_di() const noexcept { return read_field<int32_t>(parameters_type_data_di_offset_); }

    static DataLayout next_extra(DataLayout dp) noexcept;

    enum
    {
        no_parameters = -2,
        parameters_uninitialized = -1
    };

  private:
    void clean_extra_data_helper(DataLayout dp, int32_t shift, bool reset = false);

    DECLARE_STATIC_INIT

    static inline uint64_t method_offset_;
    static inline uint64_t size_offset_;
    static inline uint64_t data_size_offset_;
    static inline uint64_t parameters_type_data_di_offset_;
    static inline uint64_t data_offset_;
};
} // namespace hotspot::oops