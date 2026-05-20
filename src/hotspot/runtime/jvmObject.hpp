#pragma once

#include "jvm.hpp"

#define DECLARE_STATIC_INIT                                                                                            \
    static void initialize();                                                                                          \
    static inline auto static_init_ = [] {                                                                             \
        runtime::Jvm::register_post_init(initialize);                                                                  \
        return 0;                                                                                                      \
    }();
namespace \u1145
{

}
namespace hotspot::runtime
{

class JvmObject
{
  public:
    JvmObject(uint64_t addr) noexcept : addr(addr) {}

    ~JvmObject() = default;

    uint64_t address() const noexcept { return addr; }

    bool operator==(const JvmObject &other) const noexcept { return addr == other.addr; }

    explicit operator bool() const noexcept { return addr; }
    explicit operator uint64_t() const noexcept { return addr; }

    size_t hash_code() const noexcept { return std::hash<uint64_t>{}(addr); }

  protected:
    template <typename T> T read_field(uint64_t offset) const noexcept
    {
        if (!addr)
            return {};
        return Jvm::read<T>(address() + offset);
    }

    template <typename T> void write_field(uint64_t offset, const T &value) noexcept
    {
        if (!addr)
            return;
        Jvm::write<T>(address() + offset, value);
    }

    uint64_t read_compressed_klass_address_at(uint64_t addr) const noexcept
    {
        return Jvm::read_compressed_klass_address_value(addr);
    }

    std::string_view read_string_field(uint64_t offset) const noexcept
    {
        return Jvm::read_string_view_indirect_at(address() + offset);
    }

  private:
    uint64_t addr = 0;
};
} // namespace hotspot::runtime

namespace std
{
template <> struct hash<hotspot::runtime::JvmObject>
{
    size_t operator()(const hotspot::runtime::JvmObject &obj) const noexcept { return obj.hash_code(); }
};
} // namespace std