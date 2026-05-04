#pragma once

#include "instanceConstructor.hpp"

namespace hotspot::runtime
{
template <typename T>
concept derived_from_base = std::derived_from<T, JvmObject>;

template <typename T>
concept type_mapping_like = requires {
    { T::type_name } -> std::convertible_to<std::string_view>;
    typename T::type;
};

template <size_t N> struct FixedString
{
    char data[N];

    constexpr FixedString(const char (&str)[N]) noexcept
    {
        for (size_t i = 0; i < N; ++i)
            data[i] = str[i];
    }

    constexpr operator std::string_view() const noexcept { return std::string_view(data, N - 1); }
};

template <size_t N> FixedString(const char (&)[N]) -> FixedString<N>;

template <FixedString Name, derived_from_base T> struct TypeMapping
{
    static constexpr std::string_view type_name = Name;
    using type = T;
};

template <type_mapping_like... Types>
    requires(sizeof...(Types) > 0)
class VirtualConstructor : public InstanceConstructor
{
  public:
    VirtualConstructor() = default;

    std::pair<std::string_view, std::unique_ptr<JvmObject>> instantiate_wrapper_for(uint64_t addr) override
    {
        if (!addr)
            return {{}, nullptr};
        for (const auto &[name, factory] : arr)
            if (Jvm::address_type_is_equal_to_type(addr, Jvm::lookup_type(name)))
                return {name, factory(addr)};
        throw wrong_type_exception(addr);
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