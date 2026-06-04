#pragma once

#include "runtime/jvmObject.hpp"

namespace hotspot::oops
{
struct MySymbol
{
    volatile uint32_t hash_and_refcount_;
    uint16_t length_;
    char body_[2];

    void *operator new(size_t sz, int len) { return ::operator new(sizeof(MySymbol) + (len > 2 ? len - 2 : 0)); }

    MySymbol(const char *name, int len)
    {
        hash_and_refcount_ = (0u << 16) | 1u;
        length_ = len;
        body_[0] = body_[1] = 0;
        memcpy(body_, name, len);
    }
};

class Symbol : public runtime::JvmObject
{
  public:
    Symbol(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint16_t length() const noexcept { return read_field<uint16_t>(length_offset_); }
    uint8_t byte_at(size_t index) const noexcept { return read_field<uint8_t>(base_offset_ + index); }

    uint64_t identity_hash() const noexcept
    {
        return 0;
        // uint64_t addr_value = address();
        // uint64_t addr_bits = addr_value >> (VM.getVM().getLogMinObjAlignmentInBytes() + 3);
        // uint16_t length = get_length();
        // uint8_t byte0 = get_byte_at(0);
        // uint8_t byte1 = get_byte_at(1);
        // uint64_t id_hash = read_field<uint64_t>(id_hash_and_refcount_offset);
        // id_hash = id_hash >> 16;
        // return (id_hash | ((addr_bits ^ (length << 8) ^ ((byte0 << 8) | byte1)) << 16));
    }

    std::string_view as_view() const noexcept { return {(const char *)address() + base_offset_, length()}; }
    std::string as_string() const { return {(const char *)address() + base_offset_, length()}; }

    bool equals(std::string_view sv) const
    {
        return sv.size() == length() && !std::memcmp((void *)(address() + base_offset_), sv.data(), sv.size());
    }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t length_offset_;
    static inline uint64_t base_offset_;
    static inline uint64_t id_hash_and_refcount_offset_;
};
} // namespace hotspot::oops