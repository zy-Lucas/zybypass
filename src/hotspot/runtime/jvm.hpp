#pragma once

#include "../types/field.hpp"
#include "../types/type.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace hotspot::runtime
{
class Jvm
{
  public:
    Jvm() = delete;

    Jvm(const Jvm &) = delete;
    Jvm &operator=(const Jvm &) = delete;

    Jvm(Jvm &&) = delete;
    Jvm &operator=(Jvm &&) = delete;

    static void init();

    static uint64_t deref_symbol(const char *symbol_name);

    static std::string_view get_string_view_ref(uint64_t addr)
    {
        if (!addr)
            return {};
        return get_string_view(*(const uint64_t *)addr);
    }

    static std::string_view get_string_view(uint64_t addr) noexcept
    {
        if (!addr)
            return {};
        const char *str = (const char *)addr;
        return {str, std::strlen(str)};
    }

    static types::Type *lookup_type(std::string_view type_name, bool throw_if_not_found = false);
    static types::Type *basic_lookup_type(std::string_view type_name) noexcept
    {
        if (type_name.empty())
            return nullptr;
        auto it = name_to_type.find(type_name);
        return it != name_to_type.end() ? it->second.get() : nullptr;
    }

    static std::optional<int32_t> lookup_int_constant(std::string_view constant_name) noexcept
    {
        if (constant_name.empty())
            return std::nullopt;
        auto it = name_to_int_constant.find(constant_name);
        return it != name_to_int_constant.end() ? std::make_optional(it->second) : std::nullopt;
    }

    static std::optional<int64_t> lookup_long_constant(std::string_view constant_name) noexcept
    {
        if (constant_name.empty())
            return std::nullopt;
        auto it = name_to_long_constant.find(constant_name);
        return it != name_to_long_constant.end() ? std::make_optional(it->second) : std::nullopt;
    }

    static std::optional<uint64_t> get_vtbl_for_type(types::Type *type);

    static types::Type *find_dynamic_type_for_address(uint64_t addr, types::Type *base_type);

  private:
    static inline std::unordered_map<std::string_view, std::unique_ptr<types::Type>> name_to_type;
    static inline std::unordered_map<std::string_view, int32_t> name_to_int_constant;
    static inline std::unordered_map<std::string_view, int64_t> name_to_long_constant;
    static inline std::unordered_map<types::Type *, std::optional<uint64_t>> type_to_vtbl;
    static inline std::unordered_map<uint64_t, types::Type *> vtbl_to_type;

    static void read_vm_types();
    static void read_vm_structs();
    static void read_vm_int_constants();
    static void read_vm_long_constants();

    static types::Type *lookup_or_fail(std::string_view type_name) { return lookup_type(type_name, true); }
    static types::Type *lookup_type_or_creat_type(std::string_view type_name, size_t size, bool is_oop_type,
                                                  bool is_integer_type, bool is_unsigned)
    {
        types::Type *type = lookup_type(type_name, false);
        return type ? type : createBasicType(type_name, size, is_oop_type, is_integer_type, is_unsigned);
    };

    static types::Type *createBasicType(std::string_view type_name, size_t size, bool is_oop_type, bool is_integer_type,
                                        bool is_unsigned);

    static std::string_view trim(std::string_view sv) noexcept
    {
        while (!sv.empty() && std::isspace((unsigned char)sv.front()))
            sv.remove_prefix(1);
        while (!sv.empty() && std::isspace((unsigned char)sv.back()))
            sv.remove_suffix(1);
        return sv;
    }

    static types::Type *recursive_create_pointer_type(std::string_view type_name);
    static bool is_pointer_type(std::string_view type_name) noexcept;

#ifdef _WIN32
    static HMODULE get_jvm_handle() noexcept;
#endif

    static std::string vtbl_symbol_for_type(types::Type *type);
};
} // namespace hotspot::runtime