#pragma once

#include "instanceConstructor.hpp"
#include "jvm.hpp"
#include "jvmObject.hpp"

namespace hotspot::runtime
{
template <typename T>
concept derived_from_base = std::derived_from<T, JvmObjectBase>;

template <typename T>
concept unknown_policy = std::same_as<T, std::nullopt_t> || derived_from_base<T>;

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

template <unknown_policy unknown_t, type_mapping_like... Types>
    requires(sizeof...(Types) > 0)
class VirtualBaseConstructor : public InstanceConstructor
{
  public:
    VirtualBaseConstructor(types::Type *base_type) noexcept : base_type(base_type) {}

    std::pair<std::string_view, JvmObjectBase> instantiate_wrapper_for(uint64_t addr) override
    {
        if (!addr)
            return {{}, 0};
        types::Type *type = Jvm::find_dynamic_type_for_address(addr, base_type);
        if (!type)
            return {{}, 0};
        std::string_view sv = type->get_name();
        for (const auto &[first, second] : arr)
            if (first == sv)
                return {sv, second(addr)};
        if constexpr (!std::is_same_v<unknown_t, std::nullopt_t>)
            return {sv, unknown_t(addr)};
        throw wrong_type_exception(addr);
    }

  private:
    static constexpr std::array arr{
        std::pair{Types::type_name, +[](uint64_t addr) -> JvmObjectBase { return typename Types::type(addr); }}...};
    types::Type *base_type;
};
} // namespace hotspot::runtime