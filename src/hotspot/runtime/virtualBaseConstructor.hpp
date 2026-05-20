#pragma once

#include "instanceConstructor.hpp"

namespace hotspot::runtime
{
template <typename T>
concept unknown_policy = std::same_as<T, std::nullopt_t> || derived_from_base<T>;

template <unknown_policy unknown_t, type_mapping_like... Types>
    requires(sizeof...(Types) > 0)
class VirtualBaseConstructor : public InstanceConstructor
{
  public:
    VirtualBaseConstructor(types::Type *base_type) noexcept : base_type(base_type) {}

    std::pair<std::string_view, JvmObjectPtr> instantiate_wrapper_for(uint64_t addr) override
    {
        if (!addr)
            return {{}, {nullptr, nullptr}};
        types::Type *type = Jvm::find_dynamic_type_for_address(addr, base_type);
        if (!type)
            return {{}, {nullptr, nullptr}};
        std::string_view sv = type->name();
        for (const auto &[first, second] : arr_)
            if (first == sv)
                return {sv, second(addr)};
        if constexpr (!std::is_same_v<unknown_t, std::nullopt_t>)
            return {sv, std::make_unique<unknown_t>(addr)};
        throw create_wrong_type_exception(addr);
    }

  private:
    static constexpr std::array arr_{std::pair{Types::type_name, +[](uint64_t addr) -> JvmObjectPtr {
                                                  using ConcreteType = typename Types::type;
                                                  return JvmObjectPtr{new ConcreteType(addr), +[](JvmObject *p) {
                                                                          delete static_cast<ConcreteType *>(p);
                                                                      }};
                                              }}...};
    types::Type *base_type;
};
} // namespace hotspot::runtime