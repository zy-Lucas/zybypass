#pragma once

#include "jvm.hpp"

namespace hotspot::runtime
{
class JvmObjectBase
{
  public:
    JvmObjectBase(uint64_t addr) noexcept : addr(addr) {}

    ~JvmObjectBase() = default;

    uint64_t address() const noexcept { return addr; }

    bool operator==(const JvmObjectBase &other) const { return addr == other.addr; }
    bool operator!=(const JvmObjectBase &other) const { return !operator==(other); }

  private:
    uint64_t addr;
};

template <typename Derived> class JvmObject : public JvmObjectBase
{
  public:
    JvmObject(uint64_t addr) : JvmObjectBase(addr)
    {
        std::call_once(Derived::init_flag_, []() { Derived::initialize(); });
    }

    ~JvmObject() = default;

  protected:
    template <typename T> T read_field(size_t offset) const noexcept { return *(T *)(address() + offset); }

    template <typename T> void write_field(size_t offset, const T &value) noexcept
    {
        memcpy(address() + offset, &value, sizeof(value));
    }

    std::string_view read_string_field(uint64_t offset) const { return Jvm::get_string_view_ref(address() + offset); }
};
} // namespace hotspot::runtime