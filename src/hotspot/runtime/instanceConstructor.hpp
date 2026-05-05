#pragma once

#include "../types/wrongTypeException.h"
#include "jvmObject.hpp"

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

using JvmObjectPtr = std::unique_ptr<JvmObject, void (*)(JvmObject *)>;

class InstanceConstructor
{
  public:
    virtual ~InstanceConstructor() = default;

    virtual std::pair<std::string_view, JvmObjectPtr> instantiate_wrapper_for(uint64_t addr) = 0;

  protected:
    types::WrongTypeException create_wrong_type_exception(uint64_t addr)
    {
        return types::WrongTypeException("No suitable match for type of address " + std::to_string(addr));
    }
};
} // namespace hotspot::runtime