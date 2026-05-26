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
    static void register_post_init(void (*func)()) { post_init_callbacks().emplace_back(func); }

    static types::Type *lookup_type(std::string_view type_name, bool throw_if_not_found = false);
    static types::Type *lookup_type_or_null(std::string_view type_name) noexcept;

    static std::optional<int32_t> lookup_int_constant(std::string_view constant_name) noexcept;
    static std::optional<int64_t> lookup_long_constant(std::string_view constant_name) noexcept;

    static std::optional<uint64_t> vtbl_for_type(types::Type *type);

    static bool address_matches_type(uint64_t addr, types::Type *type);
    static types::Type *find_dynamic_type_for_address(uint64_t addr, types::Type *base_type);

    static uint64_t deref_symbol(const char *symbol_name);

    static std::string_view read_string_view_indirect_at(uint64_t addr) noexcept;
    static std::string_view read_string_view_at(uint64_t addr) noexcept;

    template <typename T> static T read(uint64_t addr, size_t size = sizeof(T)) noexcept;
    template <typename T> static void write(uint64_t addr, const T &value) noexcept;

    static uint64_t read_compressed_oop_address_value(uint64_t addr) noexcept;
    static uint64_t read_compressed_klass_address_value(uint64_t addr) noexcept;

    static bool is_client_compiler() noexcept { return using_client_compiler_; }
    static bool is_server_compiler() noexcept { return using_server_compiler_; }

    static bool is_core() noexcept { return !(using_client_compiler_ || using_server_compiler_); }

    static int32_t bytes_per_word() noexcept { return bytes_per_word_; }
    static int32_t heap_word_size() noexcept { return heap_word_size_; }
    static int32_t heap_oop_size() noexcept { return heap_oop_size_; }
    static int32_t oop_size() noexcept { return oop_size_; }

    static int32_t invocation_entry_bci() noexcept { return invocation_entry_bic_; }

    static bool is_compressed_oops_enabled() noexcept;
    static bool is_compressed_klass_pointers_enabled() noexcept;

    static int32_t object_alignment_in_bytes() noexcept;

    static constexpr uint64_t align_up(uint64_t size, uint64_t align) noexcept { return (size + align - 1) & -align; }
    static constexpr uint64_t align_down(uint64_t size, uint64_t align) noexcept { return size & ~(align - 1); }

    static constexpr uint32_t build_int_from_shorts(uint16_t low, uint16_t high) noexcept { return (high << 16) | low; }
    static constexpr uint64_t build_long_from_intsPD(uint32_t one_half, uint32_t other_half) noexcept;

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
        uint64_t addr_;
        std::string_view name_;
        CmdFlagTypes type_;
        int32_t flags_;

        int32_t origin() const noexcept { return flags_ & flags_value_origin_mask_; }

        bool is_bool() const noexcept { return type_ == BOOL; }
        bool as_bool() const noexcept { return read<bool>(addr_); }

        bool is_int() const noexcept { return type_ == INT; }
        int32_t get_int() const noexcept { return read<int32_t>(addr_); }

        bool is_uint() const noexcept { return type_ == UINT; }
        uint32_t as_uint() const noexcept { return read<uint32_t>(addr_); }

        bool is_intx() const noexcept { return type_ == INTX; }
        int64_t as_intx() const noexcept { return read<int64_t>(addr_); }

        bool is_uintx() const noexcept { return type_ == UINTX; }
        uint64_t as_uintx() const noexcept { return read<uint64_t>(addr_); }

        bool is_uint64t() const noexcept { return type_ == UINT64_T; }
        uint64_t as_uint64t() const noexcept { return read<uint64_t>(addr_); }

        bool is_sizet() const noexcept { return type_ == SIZE_T; }
        uint64_t as_sizet() const noexcept { return read<uint64_t>(addr_); }

        bool is_double() const noexcept { return type_ == DOUBLE; }
        double as_double() const noexcept { return read<double>(addr_); }

        bool is_ccstr() const noexcept { return type_ == CCSTR; }
        std::string_view as_ccstr_view() const noexcept { return read_string_view_indirect_at(addr_); }
        std::string as_ccstr() const { return std::string{read_string_view_at(addr_)}; }

        bool is_ccstrlist() const noexcept { return type_ == CCSTRLIST; }
        std::string_view as_ccstrlist_view() const noexcept { return read_string_view_indirect_at(addr_); }
        std::string as_ccstrlist() const { return std::string{read_string_view_at(addr_)}; }
    };

  private:
    static std::vector<void (*)()> &post_init_callbacks();

    static void read_vm_types();
    static void read_vm_structs();
    static void read_vm_int_constants();
    static void read_vm_long_constants();

    static void read_command_line_flags();

    static types::Type *lookup_or_fail(std::string_view type_name) { return lookup_type(type_name, true); }
    static types::Type *lookup_or_create_type(std::string_view type_name, size_t size, bool is_oop_type,
                                              bool is_integer_type, bool is_unsigned);

    static types::Type *create_basic_type(std::string_view type_name, size_t size, bool is_oop_type,
                                          bool is_integer_type, bool is_unsigned);
public:
    static std::optional<Jvm::Flag> lookup_command_line_flag(std::string_view name) noexcept;

    static std::string_view trim(std::string_view sv) noexcept;

    static types::Type *recursive_create_pointer_type(std::string_view type_name);
    static bool is_pointer_type(std::string_view type_name) noexcept;

#ifdef _WIN32
    static HMODULE jvm_handle() noexcept;
#endif

    static std::string vtbl_symbol_for_type(types::Type *type);

    static inline std::unordered_map<std::string_view, std::unique_ptr<types::Type>> name_to_type_;
    static inline std::unordered_map<std::string_view, int32_t> name_to_int_constant_;
    static inline std::unordered_map<std::string_view, int64_t> name_to_long_constant_;

    static inline std::unordered_map<types::Type *, std::optional<uint64_t>> type_to_vtbl_;
    static inline std::unordered_map<uint64_t, types::Type *> vtbl_to_type_;

    static inline std::unordered_map<std::string_view, Flag> flags_map_;

    static inline bool using_client_compiler_;
    static inline bool using_server_compiler_;

    static inline int32_t bytes_per_word_;
    static inline int32_t heap_word_size_;
    static inline int32_t heap_oop_size_;
    static inline int32_t oop_size_;

    static inline int32_t flags_default_;
    static inline int32_t flags_command_line_;
    static inline int32_t flags_environ_var_;
    static inline int32_t flags_config_file_;
    static inline int32_t flags_management_;
    static inline int32_t flags_ergonomic_;
    static inline int32_t flags_attach_on_demand_;
    static inline int32_t flags_internal_;
    static inline int32_t flags_jimage_resource_;
    static inline int32_t flags_value_origin_mask_;
    static inline int32_t flags_was_set_on_command_line_;

    static inline int32_t invocation_entry_bic_;
};

template <typename T> T Jvm::read(uint64_t addr, size_t size) noexcept
{
    if (!addr || !size || size > sizeof(T))
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

constexpr uint64_t Jvm::build_long_from_intsPD(uint32_t one_half, uint32_t other_half) noexcept
{
    if constexpr (is_big_endian())
        return ((uint64_t)one_half << 32) | other_half;
    else
        return ((uint64_t)other_half << 32) | one_half;
}
} // namespace hotspot::runtime