#pragma once

#include "runtime/jvmObject.hpp"

namespace hotspot::utilities
{
class GenericGrowableArray : public runtime::JvmObject
{
  public:
    GenericGrowableArray(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    int32_t length() const noexcept { return read_field<int32_t>(len_offset_); }
    int32_t max() const noexcept { return read_field<int32_t>(max_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t len_offset_;
    static inline uint64_t max_offset_;

  protected:
    static inline uint64_t data_offset_;
};

template <typename T> class GrowableArray : public GenericGrowableArray
{
  public:
    GrowableArray(uint64_t addr) noexcept : GenericGrowableArray(addr) {}

    T at(uint32_t index) const noexcept
    {
        if (index >= length())
            return {0};
        return T{runtime::Jvm::read<uint64_t>(data() + index * sizeof(void *))};
    }

    uint64_t data() const noexcept { return read_field<uint64_t>(data_offset_); }
};
} // namespace hotspot::utilities