#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::utilities
{
class GenericGrowableArray : public runtime::JvmObject
{
  public:
    GenericGrowableArray(uint64_t addr) : runtime::JvmObject(addr) {}

    int32_t length() const noexcept { return read_field<int32_t>(len_offset); }
    int32_t max() const noexcept { return read_field<int32_t>(max_offset); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t len_offset;
    static inline uint64_t max_offset;

  protected:
    static inline uint64_t data_offset;
};

template <typename T> class GrowableArray : public GenericGrowableArray
{
  public:
    GrowableArray(uint64_t addr) : GenericGrowableArray(addr) {}

    T at(uint32_t i) const noexcept
    {
        if (i >= length())
            return {0};
        return {read<uint64_t>(get_data() + i * sizeof(void *))};
    }

    uint64_t get_data() const noexcept { return read_field<uint64_t>(data_offset); }
};
} // namespace hotspot::utilities