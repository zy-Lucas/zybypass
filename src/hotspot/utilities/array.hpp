#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::oops
{
class Klass;
class Method;
}

namespace hotspot::utilities
{
template <typename T> struct ArrayTypeTraits;

#define DEFINE_ARRAY_TRAITS(T, ArrName, ElemName)                                                                      \
    template <> struct ArrayTypeTraits<T>                                                                              \
    {                                                                                                                  \
        static constexpr const char *array_name = ArrName;                                                             \
        static constexpr const char *elem_name = ElemName;                                                             \
    }

DEFINE_ARRAY_TRAITS(uint8_t, "Array<u1>", "u1");
DEFINE_ARRAY_TRAITS(uint16_t, "Array<u2>", "u2");
DEFINE_ARRAY_TRAITS(int32_t, "Array<int>", "int");

#undef DEFINE_ARRAY_TRAITS

class GenericArray : public runtime::JvmObject
{
  public:
    GenericArray(uint64_t addr, uint64_t data_offset) : runtime::JvmObject(addr), data_offset(data_offset) {}

    int32_t length() const noexcept { return read_field<int32_t>(length_offset); }

    virtual types::Type *get_elem_type() const noexcept = 0;

    uint64_t get_size() const noexcept;

  protected:
    int64_t get_integer_at(uint32_t index) const noexcept;
    uint64_t get_address_at(uint32_t index) const noexcept;

  private:
    uint64_t byte_sizeof(int32_t length) const noexcept { return length * get_elem_type()->get_size(); }

    DECLARE_STATIC_INIT

    static inline uint64_t length_offset;

    uint64_t data_offset;
};

class MethodArray : public GenericArray
{
  public:
    MethodArray(uint64_t addr) : GenericArray(addr, data_offset) {}

    oops::Method at(uint32_t i) const noexcept;

    types::Type *get_elem_type() const noexcept override { return elem_type; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t data_offset;
    static inline types::Type *elem_type;
};

class KlassArray : public GenericArray
{
  public:
    KlassArray(uint64_t addr) : GenericArray(addr, data_offset) {}

    oops::Klass at(uint32_t i) const noexcept;

    types::Type *get_elem_type() const noexcept override { return elem_type; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t data_offset;
    static inline types::Type *elem_type;
};

template <typename T> class IntegerArray : public GenericArray
{
  public:
    IntegerArray(uint64_t addr) : GenericArray(addr, data_offset) {}

    T at(uint32_t i) const noexcept { return get_integer_at(i); }

    types::Type *get_elem_type() const noexcept override { return elem_type; }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t data_offset;
    static inline types::Type *elem_type;
};

extern template class IntegerArray<uint8_t>;
extern template class IntegerArray<uint16_t>;
extern template class IntegerArray<int32_t>;

using U1Array = IntegerArray<uint8_t>;
using U2Array = IntegerArray<uint16_t>;
using IntArray = IntegerArray<int32_t>;
} // namespace hotspot::utilities