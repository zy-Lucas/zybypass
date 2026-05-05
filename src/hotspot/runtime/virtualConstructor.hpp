#pragma once

#include "instanceConstructor.hpp"

namespace hotspot::runtime
{
template <type_mapping_like... Types>
    requires(sizeof...(Types) > 0)
class VirtualConstructor : public InstanceConstructor
{
  public:
    VirtualConstructor() = default;

    std::pair<std::string_view, JvmObjectPtr> instantiate_wrapper_for(uint64_t addr) override
    {
        if (!addr)
            return {{}, {nullptr, nullptr}};
        for (const auto &[name, factory] : arr)
            if (Jvm::address_type_is_equal_to_type(addr, Jvm::lookup_type(name)))
                return {name, factory(addr)};
        throw create_wrong_type_exception(addr);
    }

  private:
    static constexpr std::array arr{std::pair{Types::type_name, +[](uint64_t addr) -> JvmObjectPtr {
                                                  using ConcreteType = typename Types::type;
                                                  return JvmObjectPtr{new ConcreteType(addr), +[](JvmObject *p) {
                                                                          delete static_cast<ConcreteType *>(p);
                                                                      }};
                                              }}...};
};
} // namespace hotspot::runtime