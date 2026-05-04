#pragma once

#include "../types/field.hpp"
#include "../types/type.hpp"
#include <bit>
#include <vector>

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
    static void register_post_init(void (*func)()) { get_post_init_callbacks().emplace_back(func); }

    static types::Type *lookup_type(std::string_view type_name, bool throw_if_not_found = false);
    static types::Type *basic_lookup_type(std::string_view type_name) noexcept;

    static std::optional<int32_t> lookup_int_constant(std::string_view constant_name) noexcept;
    static std::optional<int64_t> lookup_long_constant(std::string_view constant_name) noexcept;

    static std::optional<uint64_t> get_vtbl_for_type(types::Type *type);

    static bool address_type_is_equal_to_type(uint64_t addr, types::Type *type);
    static types::Type *find_dynamic_type_for_address(uint64_t addr, types::Type *base_type);

    static uint64_t deref_symbol(const char *symbol_name);

    static std::string_view get_string_view_ref(uint64_t addr) noexcept;
    static std::string_view get_string_view(uint64_t addr) noexcept;

    template <typename T> static T read(uint64_t addr, size_t size = sizeof(T)) noexcept;
    template <typename T> static void write(uint64_t addr, const T &value) noexcept;

    static uint64_t read_comp_klass_address_value(uint64_t addr) noexcept;

    static bool is_client_compiler() noexcept { return using_client_compiler; }
    static bool is_server_compiler() noexcept { return using_server_compiler; }

    static bool is_core() noexcept { return (!(using_client_compiler || using_server_compiler)); }

    static int32_t get_bytes_per_word() noexcept { return bytes_per_word; }
    static int32_t get_oop_size() noexcept { return oop_size; }

    static int32_t get_invocation_entry_bic() noexcept { return invocation_entry_bic; }

    static bool is_compressed_klass_pointers_enabled();

    static constexpr uint64_t align_up(uint64_t size, uint64_t align) noexcept { return (size + align - 1) & -align; }
    static constexpr uint64_t align_down(uint64_t size, uint64_t align) noexcept { return size & ~(align - 1); }

    static constexpr uint64_t build_long_from_intsPD(uint32_t oneHalf, uint32_t otherHalf) noexcept;

    static constexpr bool is_big_endian() noexcept { return std::endian::native == std::endian::big; }

    enum CmdFlagTypes : uint32_t
    {
        BOOL,
        INT,
        UINT,
        INTX,
        UINTX,
        UINT64_T,
        SIZE_T,
        DOUBLE,
        CCSTR,
        CCSTRLIST
    };

    struct Flag
    {
        uint64_t addr;
        std::string_view name;
        CmdFlagTypes type;
        int32_t flags;

        int32_t get_origin() const noexcept { return flags & Flags_VALUE_ORIGIN_MASK; }

        bool is_bool() const noexcept { return type == BOOL; }
        bool get_bool() const noexcept { return read<bool>(addr); }

        bool is_int() const noexcept { return type == INT; }
        int32_t get_int() const noexcept { return read<int32_t>(addr); }

        bool is_uint() const noexcept { return type == UINT; }
        uint32_t get_uint() const noexcept { return read<uint32_t>(addr); }

        bool is_intx() const noexcept { return type == INTX; }
        int64_t get_intx() const noexcept { return read<int64_t>(addr); }

        bool is_uintx() const noexcept { return type == UINTX; }
        uint64_t get_uintx() const noexcept { return read<uint64_t>(addr); }

        bool is_uint64t() const noexcept { return type == UINT64_T; }
        uint64_t get_uint64t() const noexcept { return read<uint64_t>(addr); }

        bool is_sizet() const noexcept { return type == SIZE_T; }
        uint64_t get_sizet() const noexcept { return read<uint64_t>(addr); }

        bool is_double() const noexcept { return type == DOUBLE; }
        double get_double() const noexcept { return read<double>(addr); }

        bool is_ccstr() const noexcept { return type == CCSTR; }
        std::string_view get_ccstr_view() const noexcept { return get_string_view_ref(addr); }
        std::string get_ccstr() const { return std::string{get_string_view(addr)}; }

        bool is_ccstrlist() const noexcept { return type == CCSTRLIST; }
        std::string_view get_ccstrlist_view() const noexcept { return get_string_view_ref(addr); }
        std::string get_ccstrlist() const { return std::string{get_string_view(addr)}; }
    };

  private:
    static std::vector<void (*)()> &get_post_init_callbacks();

    static void read_vm_types();
    static void read_vm_structs();
    static void read_vm_int_constants();
    static void read_vm_long_constants();

    static void read_command_line_flags();

    static types::Type *lookup_or_fail(std::string_view type_name) { return lookup_type(type_name, true); }
    static types::Type *lookup_type_or_create_type(std::string_view type_name, size_t size, bool is_oop_type,
                                                   bool is_integer_type, bool is_unsigned);

    static types::Type *createBasicType(std::string_view type_name, size_t size, bool is_oop_type, bool is_integer_type,
                                        bool is_unsigned);

    static std::optional<Jvm::Flag> get_command_line_flag(std::string_view name);

    static std::string_view trim(std::string_view sv) noexcept;

    static types::Type *recursive_create_pointer_type(std::string_view type_name);
    static bool is_pointer_type(std::string_view type_name) noexcept;

#ifdef _WIN32
    static HMODULE get_jvm_handle() noexcept;
#endif

    static std::string vtbl_symbol_for_type(types::Type *type);

    static inline std::unordered_map<std::string_view, std::unique_ptr<types::Type>> name_to_type;
    static inline std::unordered_map<std::string_view, int32_t> name_to_int_constant;
    static inline std::unordered_map<std::string_view, int64_t> name_to_long_constant;

    static inline std::unordered_map<types::Type *, std::optional<uint64_t>> type_to_vtbl;
    static inline std::unordered_map<uint64_t, types::Type *> vtbl_to_type;

    static inline std::unordered_map<std::string_view, Flag> flags_map;

    static inline bool using_client_compiler;
    static inline bool using_server_compiler;

    static inline int32_t bytes_per_word;
    static inline int32_t oop_size;

    static inline int32_t Flags_DEFAULT;
    static inline int32_t Flags_COMMAND_LINE;
    static inline int32_t Flags_ENVIRON_VAR;
    static inline int32_t Flags_CONFIG_FILE;
    static inline int32_t Flags_MANAGEMENT;
    static inline int32_t Flags_ERGONOMIC;
    static inline int32_t Flags_ATTACH_ON_DEMAND;
    static inline int32_t Flags_INTERNAL;
    static inline int32_t Flags_JIMAGE_RESOURCE;
    static inline int32_t Flags_VALUE_ORIGIN_MASK;
    static inline int32_t Flags_WAS_SET_ON_COMMAND_LINE;

    static inline int32_t invocation_entry_bic;
};

template <typename T> T Jvm::read(uint64_t addr, size_t size) noexcept
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

template <typename T> void Jvm::write(uint64_t addr, const T &value) noexcept
{
    if (!addr)
        return;
    std::memcpy((void *)addr, &value, sizeof(T));
}

constexpr uint64_t Jvm::build_long_from_intsPD(uint32_t oneHalf, uint32_t otherHalf) noexcept
{
    if constexpr (is_big_endian())
        return ((uint64_t)oneHalf << 32) | otherHalf;
    else
        return ((uint64_t)otherHalf << 32) | oneHalf;
}
} // namespace hotspot::runtime