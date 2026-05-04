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
    template <typename T> T read_field(uint64_t offset) const noexcept { return Jvm::read<T>(address() + offset); }

    template <typename T> void write_field(uint64_t offset, const T &value) noexcept
    {
        Jvm::write<T>(address() + offset, value);
    }

    uint64_t read_comp_klass_address_at(uint64_t addr) noexcept { return Jvm::read_comp_klass_address_value(addr); }

    std::string_view read_string_field(uint64_t offset) const noexcept
    {
        return Jvm::get_string_view_ref(address() + offset);
    }

  private:
    uint64_t addr;
};
} // namespace hotspot::runtime