#include "jvm.hpp"
#include "../oops/compressedKlassPointers.hpp"
#include "../oops/compressedOops.hpp"

#ifndef _WIN32
#include "../debugger/aarch64/symbol_lookup.h"
#else
#include <windows.h>
#endif

namespace hotspot::runtime
{
void Jvm::init()
{
    name_to_type_.reserve(4096);
    read_vm_types();
    read_vm_structs();
    read_vm_int_constants();
    read_vm_long_constants();

    read_command_line_flags();

    if (types::Type *type = lookup_type("Method"); type->lookup_field("_from_compiled_entry"))
    {
        if (lookup_type("Matcher"))
            using_server_compiler_ = true;
        else
            using_client_compiler_ = true;
    }
    bytes_per_word_ = *lookup_int_constant("BytesPerWord");
    heap_word_size_ = *lookup_int_constant("HeapWordSize");
    heap_oop_size_ = is_compressed_oops_enabled() ? sizeof(int32_t) : oop_size();
    oop_size_ = *lookup_int_constant("oopSize");

    flags_default_ = *lookup_int_constant("JVMFlagOrigin::DEFAULT");
    flags_command_line_ = *lookup_int_constant("JVMFlagOrigin::COMMAND_LINE");
    flags_environ_var_ = *lookup_int_constant("JVMFlagOrigin::ENVIRON_VAR");
    flags_config_file_ = *lookup_int_constant("JVMFlagOrigin::CONFIG_FILE");
    flags_management_ = *lookup_int_constant("JVMFlagOrigin::MANAGEMENT");
    flags_ergonomic_ = *lookup_int_constant("JVMFlagOrigin::ERGONOMIC");
    flags_attach_on_demand_ = *lookup_int_constant("JVMFlagOrigin::ATTACH_ON_DEMAND");
    flags_internal_ = *lookup_int_constant("JVMFlagOrigin::INTERNAL");
    flags_jimage_resource_ = *lookup_int_constant("JVMFlagOrigin::JIMAGE_RESOURCE");
    flags_value_origin_mask_ = *lookup_int_constant("JVMFlag::VALUE_ORIGIN_MASK");
    flags_was_set_on_command_line_ = *lookup_int_constant("JVMFlag::WAS_SET_ON_COMMAND_LINE");

    invocation_entry_bic_ = *lookup_int_constant("InvocationEntryBci");

    reserve_for_allocation_prefetch_ =
        read<int32_t>(*lookup_type("ThreadLocalAllocBuffer")->field_offset("_reserve_for_allocation_prefetch"));

    for (auto func : post_init_callbacks())
        func();
    post_init_callbacks().clear();
}

// string_view指向临时string就等炸吧
types::Type *Jvm::lookup_type(std::string_view type_name, bool throw_if_not_found)
{
    if (type_name.empty())
        return nullptr;
    types::Type *type = lookup_type_or_null(type_name);
    if (!type && type_name.starts_with("const "))
        type = lookup_type(type_name.substr(6));
    if (!type && type_name.ends_with(" const"))
        type = lookup_type(type_name.substr(0, type_name.size() - 6));
    if (!type)
    {
        if (type_name.starts_with("GrowableArray<") && type_name.ends_with('>'))
        {
            constexpr std::string_view prefix("GrowableArray<");
            std::string_view element_type_name{type_name.substr(prefix.size(), type_name.size() - prefix.size() - 1)};

            types::Type *element_type = lookup_type(element_type_name);
            if (!element_type && is_pointer_type(element_type_name))
                element_type = recursive_create_pointer_type(element_type_name);
            if (!element_type)
                throw std::runtime_error("Unknown type: " + std::string(element_type_name));

            auto new_type{std::make_unique<types::Type>(type_name, nullptr, -1, false, false, false)};
            types::Type *target_type = new_type.get();
            name_to_type_.try_emplace(target_type->name(), std::move(new_type));

            types::Type *generic = lookup_or_fail("GrowableArrayBase");
            types::Type *specific = lookup_or_fail("GrowableArray<int>");
            target_type->set_size(specific->size());

            for (const auto &[field_name, field] : *generic)
                target_type->add_field(std::make_unique<types::Field>(field->type_name(), field_name,
                                                                      (types::Type *)field->field_type(),
                                                                      field->is_static(), field->offset()));
            type = target_type;
        }
    }
    if (!type && is_pointer_type(type_name))
        type = recursive_create_pointer_type(type_name);
    if (!type && throw_if_not_found)
        throw std::runtime_error("Unknown type: " + std::string(type_name));
    return type;
}

types::Type *Jvm::lookup_type_or_null(std::string_view type_name) noexcept
{
    if (type_name.empty())
        return nullptr;
    auto it{name_to_type_.find(type_name)};
    return it != name_to_type_.end() ? it->second.get() : nullptr;
}

std::optional<int32_t> Jvm::lookup_int_constant(std::string_view constant_name) noexcept
{
    if (constant_name.empty())
        return std::nullopt;
    auto it{name_to_int_constant_.find(constant_name)};
    return it != name_to_int_constant_.end() ? std::make_optional(it->second) : std::nullopt;
}

std::optional<int64_t> Jvm::lookup_long_constant(std::string_view constant_name) noexcept
{
    if (constant_name.empty())
        return std::nullopt;
    auto it{name_to_long_constant_.find(constant_name)};
    return it != name_to_long_constant_.end() ? std::make_optional(it->second) : std::nullopt;
}

std::optional<uint64_t> Jvm::vtbl_for_type(types::Type *type)
{
    if (!type)
        return std::nullopt;
    if (auto result{type_to_vtbl_.find(type)}; result != type_to_vtbl_.cend())
        return result->second;
    std::string vtbl_symbol{vtbl_symbol_for_type(type)};
    uint64_t addr;
#ifdef _WIN32
    HMODULE libjvm = jvm_handle();
    if (!libjvm)
        return std::nullopt;
    addr = (uint64_t)GetProcAddress(libjvm, vtbl_symbol.c_str());
#else
    addr = debugger::aarch64::lookup_by_name(vtbl_symbol.c_str());
#endif
    if (addr)
    {
        addr = vtbl_symbol.starts_with("_ZTV") ? addr + 2 * sizeof(void *) : addr;
        vtbl_to_type_.try_emplace(addr, type);
        return type_to_vtbl_.try_emplace(type, addr).first->second;
    }
    return type_to_vtbl_.try_emplace(type, std::nullopt).first->second;
}

bool Jvm::address_matches_type(uint64_t addr, types::Type *type)
{
    if (!addr)
        return false;
    if (std::optional<uint64_t> vtable_addr{vtbl_for_type(type)}; vtable_addr && *vtable_addr == read<uint64_t>(addr))
        return true;
    return false;
}

types::Type *Jvm::find_dynamic_type_for_address(uint64_t addr, types::Type *base_type)
{
    if (!addr || !vtbl_for_type(base_type))
        return nullptr;
    static constexpr size_t ptr_size = sizeof(void *);
    uint64_t candidates[3] = {read<uint64_t>(addr), 0, 0};
    int64_t offset2 = base_type->size() - (base_type->size() % ptr_size) - ptr_size;

    if (offset2 > 0)
    {
        candidates[1] = read<uint64_t>(addr + offset2);
        if (offset2 - (int64_t)ptr_size > 0)
            candidates[2] = read<uint64_t>(addr + offset2 - ptr_size);
    }
    for (uint64_t loc : candidates)
    {
        if (!loc)
            continue;
        if (auto it{vtbl_to_type_.find(loc)}; it != vtbl_to_type_.end())
            for (const auto *super = it->second; super; super = super->super_class())
                if (super == base_type)
                    return it->second;
    }
    for (const auto &[name, type_ptr] : name_to_type_)
    {
        auto *type = type_ptr.get();
        const auto *super = type;
        while (super && super != base_type)
            super = super->super_class();
        if (!super)
            continue;
        if (std::optional<uint64_t> vtable_addr{vtbl_for_type(type)}; vtable_addr)
            if (uint64_t v = *vtable_addr; v == candidates[0] || v == candidates[1] || v == candidates[2])
                return type;
    };
    return nullptr;
}

uint64_t Jvm::deref_symbol(const char *symbol_name)
{
    if (!symbol_name || symbol_name[0] == '\0')
        return 0;
#ifdef _WIN32
    HMODULE libjvm = jvm_handle();
    if (!libjvm)
        return 0;
    if (auto addr = (uint64_t)GetProcAddress(libjvm, symbol_name.c_str()); addr)
        return read<uint64_t>(addr);
    return 0;
#else
    if (uint64_t addr = debugger::aarch64::lookup_by_name(symbol_name); addr)
        return read<uint64_t>(addr);
    return 0;
#endif
}

std::string_view Jvm::read_string_view_indirect_at(uint64_t addr) noexcept
{
    if (!addr)
        return {};
    return read_string_view_at(read<uint64_t>(addr));
}

std::string_view Jvm::read_string_view_at(uint64_t addr) noexcept
{
    if (!addr)
        return {};
    return {(const char *)addr};
}

uint64_t Jvm::read_compressed_oop_address_value(uint64_t addr) noexcept
{
    if (uint64_t value = read<uint32_t>(addr); value)
        return oops::CompressedOops::base() + (value << oops::CompressedOops::shift());
    return 0;
}

void Jvm::write_compressed_oop_address_value(uint64_t addr, uint64_t value) noexcept
{
    if (!value)
        return;
    write<uint32_t>(addr, (value - oops::CompressedOops::base()) >> oops::CompressedOops::shift());
}

uint64_t Jvm::read_compressed_klass_address_value(uint64_t addr) noexcept
{
    if (uint32_t value = read<uint32_t>(addr); value)
        return oops::CompressedKlassPointers::base() + (value << oops::CompressedKlassPointers::shift());
    return 0;
}

void Jvm::write_compressed_klass_address_value(uint64_t addr, uint64_t value) noexcept
{
    if (!value)
        return;
    write<uint32_t>(addr, (value - oops::CompressedKlassPointers::base()) >> oops::CompressedKlassPointers::shift());
}

bool Jvm::is_compressed_oops_enabled() noexcept
{
    static bool compressed_oops_enabled = lookup_command_line_flag("UseCompressedOops")
                                              .transform([](const Flag &f) { return f.as_bool(); })
                                              .value_or(false);
    return compressed_oops_enabled;
}

bool Jvm::is_compressed_klass_pointers_enabled() noexcept
{
    static bool compressed_klass_pointers_enabled = lookup_command_line_flag("UseCompressedClassPointers")
                                                        .transform([](const Flag &f) { return f.as_bool(); })
                                                        .value_or(false);
    return compressed_klass_pointers_enabled;
}

int32_t Jvm::min_obj_alignment_in_bytes() noexcept
{
    static int32_t object_alignment_in_bytes = lookup_command_line_flag("ObjectAlignmentInBytes")
                                                   .transform([](const Flag &f) { return f.as_intx(); })
                                                   .value_or(8);
    return object_alignment_in_bytes;
}

int32_t Jvm::code_entry_alignment() noexcept
{
#if defined(__aarch64__)
    return 64;
#elif defined(__x86_64__) || defined(_M_X64)
    return 16;
#elif defined(__i386__) || defined(_M_IX86)
    return 32;
#elif defined(__riscv) && (__riscv_xlen == 64)
    return 64;
#elif defined(__powerpc64__) || defined(__ppc64__)
    return 128;
#elif defined(__s390x__)
    return 64;
#elif defined(__arm__)
    return 16;
#else
    return 64;
#endif
}

std::vector<void (*)()> &Jvm::post_init_callbacks()
{
    static std::vector<void (*)()> callbacks;
    return callbacks;
}

void Jvm::read_vm_types()
{
    uint64_t entry_addr = deref_symbol("gHotSpotVMTypes");
    uint64_t type_entry_type_name_offset = deref_symbol("gHotSpotVMTypeEntryTypeNameOffset");
    uint64_t type_entry_superclass_name_offset = deref_symbol("gHotSpotVMTypeEntrySuperclassNameOffset");
    uint64_t type_entry_is_oop_type_offset = deref_symbol("gHotSpotVMTypeEntryIsOopTypeOffset");
    uint64_t type_entry_is_integer_type_offset = deref_symbol("gHotSpotVMTypeEntryIsIntegerTypeOffset");
    uint64_t type_entry_is_unsigned_offset = deref_symbol("gHotSpotVMTypeEntryIsUnsignedOffset");
    uint64_t type_entry_size_offset = deref_symbol("gHotSpotVMTypeEntrySizeOffset");
    uint64_t type_entry_array_stride = deref_symbol("gHotSpotVMTypeEntryArrayStride");

    for (;; entry_addr += type_entry_array_stride)
    {
        std::string_view type_name{read_string_view_indirect_at(entry_addr + type_entry_type_name_offset)};
        if (type_name.empty())
            break;
        std::string_view superclass_name{read_string_view_indirect_at(entry_addr + type_entry_superclass_name_offset)};
        bool is_oop_type = read<int32_t>(entry_addr + type_entry_is_oop_type_offset);
        bool is_integer_type = read<int32_t>(entry_addr + type_entry_is_integer_type_offset);
        bool is_unsigned = read<int32_t>(entry_addr + type_entry_is_unsigned_offset);
        uint64_t size = read<uint64_t>(entry_addr + type_entry_size_offset);

        auto type{std::make_unique<types::Type>(type_name,
                                                lookup_or_create_type(superclass_name, -1, false, false, false), size,
                                                is_oop_type, is_integer_type, is_unsigned)};
        name_to_type_.try_emplace(type_name, std::move(type));
    }
}

void Jvm::read_vm_structs()
{
    uint64_t entry_addr = deref_symbol("gHotSpotVMStructs");
    uint64_t struct_entry_type_name_offset = deref_symbol("gHotSpotVMStructEntryTypeNameOffset");
    uint64_t struct_entry_field_name_offset = deref_symbol("gHotSpotVMStructEntryFieldNameOffset");
    uint64_t struct_entry_type_string_offset = deref_symbol("gHotSpotVMStructEntryTypeStringOffset");
    uint64_t struct_entry_is_static_offset = deref_symbol("gHotSpotVMStructEntryIsStaticOffset");
    uint64_t struct_entry_offset_offset = deref_symbol("gHotSpotVMStructEntryOffsetOffset");
    uint64_t struct_entry_address_offset = deref_symbol("gHotSpotVMStructEntryAddressOffset");
    uint64_t struct_entry_array_stride = deref_symbol("gHotSpotVMStructEntryArrayStride");

    name_to_type_.try_emplace("<opaque>", std::make_unique<types::Type>("<opaque>", nullptr, -1, false, false, false));

    for (;; entry_addr += struct_entry_array_stride)
    {
        std::string_view field_name{read_string_view_indirect_at(entry_addr + struct_entry_field_name_offset)};
        std::string_view type_name{read_string_view_indirect_at(entry_addr + struct_entry_type_name_offset)};
        if (field_name.empty())
            break;
        std::string_view type_string{read_string_view_indirect_at(entry_addr + struct_entry_type_string_offset)};
        if (type_string.empty())
            type_string = "<opaque>";
        bool is_static = read<int32_t>(entry_addr + struct_entry_is_static_offset);
        uint64_t offset =
            read<uint64_t>(entry_addr + (is_static ? struct_entry_address_offset : struct_entry_offset_offset));

        if (types::Type *type = lookup_type(type_name); type)
            type->add_field(
                std::make_unique<types::Field>(type_name, field_name, lookup_type(type_string), is_static, offset));
    }
}

void Jvm::read_vm_int_constants()
{
    uint64_t entry_addr = deref_symbol("gHotSpotVMIntConstants");
    uint64_t int_constant_entry_name_offset = deref_symbol("gHotSpotVMIntConstantEntryNameOffset");
    uint64_t int_constant_entry_value_offset = deref_symbol("gHotSpotVMIntConstantEntryValueOffset");
    uint64_t int_constant_entry_array_stride = deref_symbol("gHotSpotVMIntConstantEntryArrayStride");

    for (;; entry_addr += int_constant_entry_array_stride)
    {
        std::string_view name{read_string_view_indirect_at(entry_addr + int_constant_entry_name_offset)};
        if (name.empty())
            break;
        int32_t value = read<int32_t>(entry_addr + int_constant_entry_value_offset);
        if (!lookup_int_constant(name))
            name_to_int_constant_.try_emplace(name, value);
    }
}

void Jvm::read_vm_long_constants()
{
    uint64_t entry_addr = deref_symbol("gHotSpotVMLongConstants");
    uint64_t long_constant_entry_name_offset = deref_symbol("gHotSpotVMLongConstantEntryNameOffset");
    uint64_t long_constant_entry_value_offset = deref_symbol("gHotSpotVMLongConstantEntryValueOffset");
    uint64_t long_constant_entry_array_stride = deref_symbol("gHotSpotVMLongConstantEntryArrayStride");

    for (;; entry_addr += long_constant_entry_array_stride)
    {
        std::string_view name{read_string_view_indirect_at(entry_addr + long_constant_entry_name_offset)};
        if (name.empty())
            break;
        int64_t value = read<int64_t>(entry_addr + long_constant_entry_value_offset);
        if (!lookup_long_constant(name))
            name_to_long_constant_.try_emplace(name, value);
    }
}

void Jvm::read_command_line_flags()
{
    types::Type *field_type = lookup_type("JVMFlag");

    uint64_t flag_addr = read<uint64_t>(*field_type->field_offset("flags"));
    uint64_t num_flags = read<uint64_t>(*field_type->field_offset("numFlags"));

    uint64_t type_offset = *field_type->field_offset("_type");
    uint64_t name_offset = *field_type->field_offset("_name");
    uint64_t addr_offset = *field_type->field_offset("_addr");
    uint64_t flags_offset = *field_type->field_offset("_flags");

    uint64_t flag_size = field_type->size();

    if (!num_flags)
        return;
    --num_flags;
    flags_map_.reserve(num_flags);

    for (uint32_t f = 0; f < num_flags; ++f)
    {
        uint64_t addr = read<uint64_t>(flag_addr + addr_offset);
        std::string_view name{read_string_view_indirect_at(flag_addr + name_offset)};
        int32_t type = read<int32_t>(flag_addr + type_offset);
        int32_t flags = read<int32_t>(flag_addr + flags_offset);
        flags_map_.try_emplace(name, Flag{addr, name, (CmdFlagTypes)type, flags});
        flag_addr += flag_size;
    }
}

types::Type *Jvm::lookup_or_create_type(std::string_view type_name, size_t size, bool is_oop_type, bool is_integer_type,
                                        bool is_unsigned)
{
    types::Type *type = lookup_type(type_name, false);
    return type ? type : create_basic_type(type_name, size, is_oop_type, is_integer_type, is_unsigned);
};

types::Type *Jvm::create_basic_type(std::string_view type_name, size_t size, bool is_oop_type, bool is_integer_type,
                                    bool is_unsigned)
{

    if (is_integer_type || !is_pointer_type(type_name))
    {
        auto new_type{
            std::make_unique<types::Type>(type_name, nullptr, size, is_oop_type, is_integer_type, is_unsigned)};
        types::Type *type = new_type.get();
        return name_to_type_.try_emplace(type->name(), std::move(new_type)).first->second.get();
    }
    return recursive_create_pointer_type(type_name);
}

std::optional<Jvm::Flag> Jvm::lookup_command_line_flag(std::string_view name) noexcept
{
    if (name.empty())
        return std::nullopt;
    auto it{flags_map_.find(name)};
    return it != flags_map_.end() ? std::make_optional(it->second) : std::nullopt;
}

std::string_view Jvm::trim(std::string_view sv) noexcept
{
    while (!sv.empty() && std::isspace((unsigned char)sv.front()))
        sv.remove_prefix(1);
    while (!sv.empty() && std::isspace((unsigned char)sv.back()))
        sv.remove_suffix(1);
    return sv;
}

types::Type *Jvm::recursive_create_pointer_type(std::string_view type_name)
{
    types::Type *result_type = lookup_type_or_null(type_name);
    if (result_type)
        return result_type;
    auto star_pos = type_name.find_last_of('*');
    if (star_pos == std::string_view::npos)
        return nullptr;
    std::string_view target_type_name{trim(type_name.substr(0, star_pos))};
    types::Type *target_type;

    if (is_pointer_type(target_type_name))
    {
        if (!(target_type = lookup_type(target_type_name)))
            target_type = recursive_create_pointer_type(target_type_name);
    }
    else
    {
        if (!(target_type = lookup_type(target_type_name)))
        {
            if (target_type_name == "char" || target_type_name == "const char")
                target_type = create_basic_type(target_type_name, sizeof(char), false, true, false);
            else if (target_type_name == "u_char")
                target_type = create_basic_type(target_type_name, sizeof(unsigned char), false, true, true);
            else
                target_type = create_basic_type(target_type_name, -1, false, false, false);
        }
    }
    auto new_type{
        std::make_unique<types::Type>(type_name, nullptr, sizeof(void *), false, false, false, true, target_type)};
    result_type = new_type.get();
    name_to_type_.try_emplace(result_type->name(), std::move(new_type));
    return result_type;
}

bool Jvm::is_pointer_type(std::string_view type_name) noexcept
{
    auto it{type_name.rbegin()};
    while (it != type_name.rend() && std::isspace((unsigned char)*it))
        ++it;
    return it != type_name.rend() && *it == '*';
}

#ifdef _WIN32
HMODULE Jvm::jvm_handle() noexcept
{
    static HMODULE h = GetModuleHandleA("jvm.dll");
    return h;
}
#endif

std::string Jvm::vtbl_symbol_for_type(types::Type *type)
{
#ifdef _WIN32
    return std::format("??_7{}@@6B@", type->get_name());
#else
    std::string_view vt{"_ZTV"};
    if (debugger::aarch64::lookup_by_name("_vt_10JavaThread"))
        vt = "_vt_";
    return std::format("{}{}{}", vt, type->name().size(), type->name());
#endif
}
} // namespace hotspot::runtime