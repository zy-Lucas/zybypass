#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::oops
{
struct MySymbol
{
    volatile uint32_t hash_and_refcount;
    uint16_t length;
    char body[2];

    void *operator new(size_t sz, int len)
    {
        return ::operator new(sizeof(MySymbol) + (len > 2 ? len - 2 : 0));
    }

    MySymbol(const char *name, int len)
    {
        hash_and_refcount = (0u << 16) | 1u;
        length = len;
        body[0] = body[1] = 0;
        memcpy(body, name, len);
    }
};

class Symbol : public runtime::JvmObject<Symbol>
{
  public:
    Symbol(uint64_t addr);

    uint16_t get_length() const noexcept { return read_field<uint16_t>(length_offset); }
    uint8_t get_byte_at(size_t index) const noexcept { return read_field<uint8_t>(base_offset + index); }

    std::string_view as_view() const noexcept { return {(const char *)address() + base_offset, get_length()}; }
    std::string as_string() const { return {(const char *)address() + base_offset, get_length()}; }

    bool equals(std::string_view sv) const
    {
        return sv.size() == get_length() && !std::memcmp((void *)(address() + base_offset), sv.data(), sv.size());
    }

  private:
    friend class runtime::JvmObject<Symbol>;

    static inline std::once_flag init_flag_;

    static inline uint64_t length_offset;
    static inline uint64_t base_offset;
    static inline uint64_t id_hash_and_refcount_offset;

    static void initialize();
};
} // namespace hotspot::oops