#pragma once

#include "../types/field.hpp"
#include "../types/type.hpp"
#include <bit>

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

    static types::Type *lookup_type(std::string_view type_name, bool throw_if_not_found = false);
    static types::Type *basic_lookup_type(std::string_view type_name) noexcept;

    static std::optional<int32_t> lookup_int_constant(std::string_view constant_name) noexcept;
    static std::optional<int64_t> lookup_long_constant(std::string_view constant_name) noexcept;

    static std::optional<uint64_t> get_vtbl_for_type(types::Type *type);

    static types::Type *find_dynamic_type_for_address(uint64_t addr, types::Type *base_type);

    static uint64_t deref_symbol(const char *symbol_name);

    static std::string_view get_string_view_ref(uint64_t addr);
    static std::string_view get_string_view(uint64_t addr) noexcept;

    static bool is_client_compiler() noexcept { return using_client_compiler; }
    static bool is_server_compiler() noexcept { return using_server_compiler; }

    static bool is_core() noexcept { return (!(using_client_compiler || using_server_compiler)); }

    static std::optional<int32_t> get_bytes_per_word() noexcept { return bytes_per_word; }
    static std::optional<int32_t> get_oop_size() noexcept { return oop_size; }

    static constexpr uint64_t align_up(uint64_t size, uint64_t align) noexcept { return (size + align - 1) & -align; }
    static constexpr uint64_t align_down(uint64_t size, uint64_t align) noexcept { return size & ~(align - 1); }

    static constexpr uint64_t build_long_from_intsPD(uint32_t oneHalf, uint32_t otherHalf) noexcept;

    static constexpr bool is_big_endian() noexcept { return std::endian::native == std::endian::big; }

  private:
    static inline std::unordered_map<std::string_view, std::unique_ptr<types::Type>> name_to_type;
    static inline std::unordered_map<std::string_view, int32_t> name_to_int_constant;
    static inline std::unordered_map<std::string_view, int64_t> name_to_long_constant;
    static inline std::unordered_map<types::Type *, std::optional<uint64_t>> type_to_vtbl;
    static inline std::unordered_map<uint64_t, types::Type *> vtbl_to_type;

    static inline bool using_client_compiler;
    static inline bool using_server_compiler;

    static inline std::optional<int32_t> bytes_per_word;
    static inline std::optional<int32_t> oop_size;

    static void read_vm_types();
    static void read_vm_structs();
    static void read_vm_int_constants();
    static void read_vm_long_constants();

    static types::Type *lookup_or_fail(std::string_view type_name) { return lookup_type(type_name, true); }
    static types::Type *lookup_type_or_create_type(std::string_view type_name, size_t size, bool is_oop_type,
                                                   bool is_integer_type, bool is_unsigned)
    {
        types::Type *type = lookup_type(type_name, false);
        return type ? type : createBasicType(type_name, size, is_oop_type, is_integer_type, is_unsigned);
    };

    static types::Type *createBasicType(std::string_view type_name, size_t size, bool is_oop_type, bool is_integer_type,
                                        bool is_unsigned);

    static std::string_view trim(std::string_view sv) noexcept;

    static types::Type *recursive_create_pointer_type(std::string_view type_name);
    static bool is_pointer_type(std::string_view type_name) noexcept;

#ifdef _WIN32
    static HMODULE get_jvm_handle() noexcept;
#endif

    static std::string vtbl_symbol_for_type(types::Type *type);
};

constexpr uint64_t Jvm::build_long_from_intsPD(uint32_t oneHalf, uint32_t otherHalf) noexcept
{
    if constexpr (is_big_endian())
        return ((uint64_t)oneHalf << 32) | otherHalf;
    else
        return ((uint64_t)otherHalf << 32) | oneHalf;
}
} // namespace hotspot::runtime