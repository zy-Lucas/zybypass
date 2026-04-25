#pragma once

#include "jvm.hpp"

#define DECLARE_STATIC_INIT                                                                                            \
    static void initialize();                                                                                          \
    static inline auto static_init_ = [] {                                                                             \
        runtime::Jvm::register_post_init(initialize);                                                                  \
        return 0;                                                                                                      \
    }();

#define STATIC_INIT_GUARD std::call_once(init_flag_, initialize)

namespace hotspot::runtime
{

class JvmObject
{
  public:
    JvmObject(uint64_t addr) : addr(addr) {}

    ~JvmObject() = default;

    uint64_t address() const noexcept { return addr; }

    bool operator==(const JvmObject &other) const noexcept { return addr == other.addr; }
    bool operator!=(const JvmObject &other) const noexcept { return !operator==(other); }

    explicit operator bool() const noexcept { return addr; }
    explicit operator uint64_t() const noexcept { return addr; }

  protected:
    template <typename T> T read_field(uint64_t offset) const noexcept { return read<T>(offset + address()); }

    template <typename T> void write_field(uint64_t offset, const T &value) noexcept
    {
        write<T>(offset + address(), value);
    }

    template <typename T> static T read(uint64_t addr, size_t size = sizeof(T)) noexcept
    {
        if (!addr)
            return {};
        if (size == sizeof(T))
        {
            T value{};
            std::memcpy(&value, (const void *)addr, size);
            return value;
        }
        if constexpr (!std::is_integral_v<T> || std::is_same_v<T, bool>)
            return {};
        else
        {
            using UT = std::make_unsigned_t<T>;
            UT u{};
            std::memcpy(&u, (const void *)addr, size);
            if constexpr (std::endian::native == std::endian::big)
                u >>= (sizeof(T) - size) * 8;
            if constexpr (std::is_signed_v<T>)
                if (u >> (size * 8 - 1))
                    u |= ~UT{} << (size * 8);
            return (T)u;
        }
    }

    template <typename T> static void write(uint64_t addr, const T &value) noexcept
    {
        if (!addr)
            return;
        std::memcpy((void *)addr, &value, sizeof(T));
    }

    std::string_view read_string_field(uint64_t offset) const { return Jvm::get_string_view_ref(address() + offset); }

  private:
    uint64_t addr;
};
} // namespace hotspot::runtime