#include "jvm.hpp"
#ifndef _WIN32
#include "../debugger/aarch64/symbol_lookup.h"
#else
#include <windows.h>
#endif

namespace hotspot::runtime
{
void Jvm::init()
{
    name_to_type.reserve(4096);
    read_vm_types();
    read_vm_structs();
    read_vm_int_constants();
    read_vm_long_constants();

    if (types::Type *type = lookup_type("Method"); type->get_field("_from_compiled_entry"))
    {
        if (lookup_type("Matcher"))
            using_server_compiler = true;
        else
            using_client_compiler = true;
    }
    bytes_per_word = lookup_int_constant("BytesPerWord");
    oop_size = lookup_int_constant("oopSize");
}

// string_view指向临时string就等炸吧
types::Type *Jvm::lookup_type(std::string_view type_name, bool throw_if_not_found)
{
    if (type_name.empty())
        return nullptr;
    auto type = basic_lookup_type(type_name);
    if (!type && type_name.starts_with("const "))
        type = lookup_type(type_name.substr(6));
    if (!type && type_name.ends_with(" const"))
        type = lookup_type(type_name.substr(0, type_name.size() - 6));
    if (!type)
    {
        if (type_name.starts_with("GrowableArray<") && type_name.ends_with('>'))
        {
            constexpr std::string_view prefix("GrowableArray<");
            auto element_type_name = type_name.substr(prefix.size(), type_name.size() - prefix.size() - 1);

            auto element_type = lookup_type(element_type_name);
            if (!element_type && is_pointer_type(element_type_name))
                element_type = recursive_create_pointer_type(element_type_name);
            if (!element_type)
                throw std::runtime_error("Unknown type: " + std::string(element_type_name));

            auto new_type = std::make_unique<types::Type>(type_name, nullptr, -1, false, false, false);
            auto target_type = new_type.get();
            name_to_type.try_emplace(target_type->get_name(), std::move(new_type));

            auto generic = lookup_or_fail("GrowableArrayBase");
            auto specific = lookup_or_fail("GrowableArray<int>");
            target_type->set_size(specific->get_size());

            for (const auto &[field_name, field] : *generic)
            {
                target_type->add_field(std::make_unique<types::Field>(field->get_type_name(), field_name,
                                                                      (types::Type *)field->get_field_type(),
                                                                      field->get_is_static(), field->get_offset()));
            }
            type = target_type;
        }
    }
    if (!type && is_pointer_type(type_name))
        type = recursive_create_pointer_type(type_name);
    if (!type && throw_if_not_found)
        throw std::runtime_error("Unknown type: " + std::string(type_name));
    return type;
}

types::Type *Jvm::basic_lookup_type(std::string_view type_name) noexcept
{
    if (type_name.empty())
        return nullptr;
    auto it = name_to_type.find(type_name);
    return it != name_to_type.end() ? it->second.get() : nullptr;
}

std::optional<int32_t> Jvm::lookup_int_constant(std::string_view constant_name) noexcept
{
    if (constant_name.empty())
        return std::nullopt;
    auto it = name_to_int_constant.find(constant_name);
    return it != name_to_int_constant.end() ? std::make_optional(it->second) : std::nullopt;
}

std::optional<int64_t> Jvm::lookup_long_constant(std::string_view constant_name) noexcept
{
    if (constant_name.empty())
        return std::nullopt;
    auto it = name_to_long_constant.find(constant_name);
    return it != name_to_long_constant.end() ? std::make_optional(it->second) : std::nullopt;
}

std::optional<uint64_t> Jvm::get_vtbl_for_type(types::Type *type)
{
    if (!type)
        return std::nullopt;
    auto result = type_to_vtbl.find(type);
    if (result != type_to_vtbl.cend())
        return result->second;
    std::string vtbl_symbol{vtbl_symbol_for_type(type)};
    uint64_t addr;
#ifdef _WIN32
    HMODULE libjvm = get_jvm_handle();
    if (!libjvm)
        return std::nullopt;
    addr = (uint64_t)GetProcAddress(libjvm, vtbl_symbol);
#else
    addr = lookup_by_name(vtbl_symbol.c_str());
#endif
    if (addr)
    {
        addr = vtbl_symbol.starts_with("_ZTV") ? addr + 2 * sizeof(void *) : addr;
        vtbl_to_type.try_emplace(addr, type);
        return type_to_vtbl.try_emplace(type, addr).first->second;
    }
    return type_to_vtbl.try_emplace(type, std::nullopt).first->second;
}

types::Type *Jvm::find_dynamic_type_for_address(uint64_t addr, types::Type *base_type)
{
    if (!addr || !get_vtbl_for_type(base_type))
        return nullptr;

    static constexpr size_t ptr_size = sizeof(void *);

    uint64_t candidates[3] = {*(uint64_t *)addr, 0, 0};

    int64_t offset2 = base_type->get_size() - (base_type->get_size() % ptr_size) - ptr_size;
    if (offset2 > 0)
    {
        candidates[1] = *(uint64_t *)(addr + offset2);
        if (offset2 - (int64_t)ptr_size > 0)
            candidates[2] = *(uint64_t *)(addr + offset2 - ptr_size);
    }

    for (uint64_t loc : candidates)
    {
        if (!loc)
            continue;
        auto it = vtbl_to_type.find(loc);
        if (it != vtbl_to_type.end())
            for (const auto *super = it->second; super; super = super->get_super_class())
                if (super == base_type)
                    return it->second;
    }

    for (const auto &[name, type_ptr] : name_to_type)
    {
        auto *type = type_ptr.get();
        const auto *super = type;
        while (super && super != base_type)
            super = super->get_super_class();
        if (!super)
            continue;
        if (auto vtable_addr = get_vtbl_for_type(type))
        {
            uint64_t v = *vtable_addr;
            if (v == candidates[0] || v == candidates[1] || v == candidates[2])
                return type;
        }
    };
    return nullptr;
}

uint64_t Jvm::deref_symbol(const char *symbol_name)
{
    if (!symbol_name || symbol_name[0] == '\0')
        return 0;
#ifdef _WIN32
    HMODULE libjvm = get_jvm_handle();
    if (!libjvm)
        return 0;
    uint64_t addr = (uint64_t)GetProcAddress(libjvm, symbol_name);
    if (!addr)
        return 0;
    return *(uint64_t *)(addr);
#else
    uint64_t addr = lookup_by_name(symbol_name);
    if (!addr)
        return 0;
    return *(uint64_t *)addr;
#endif
}

std::string_view Jvm::get_string_view_ref(uint64_t addr)
{
    if (!addr)
        return {};
    return get_string_view(*(const uint64_t *)addr);
}

std::string_view Jvm::get_string_view(uint64_t addr) noexcept
{
    if (!addr)
        return {};
    const char *str = (const char *)addr;
    return {str, std::strlen(str)};
}

void Jvm::read_vm_types()
{
    uint64_t entry_addr = deref_symbol("gHotSpotVMTypes");
    uint64_t type_entry_type_name_offset = deref_symbol("gHotSpotVMTypeEntryTypeNameOffset");
    uint64_t type_entry_superclass_name_offset = deref_symbol("gHotSpotVMTypeEntrySuperclassNameOffset");
    uint64_t type_entry_is_oop_type_offset = deref_symbol("gHotSpotVMTypeEntryIsOopTypeOffset");
    uint64_t type_entry_is_integer_type_offset = deref_symbol("gHotSpotVMTypeEntryIsIntegerTypeOffset");
    uint64_t type_entry_is_unsigned_offset = deref_symbol("gHotSpotVMTypeEntryIsUnsignedOffset");
    uint64_t type_entry_size_Offset = deref_symbol("gHotSpotVMTypeEntrySizeOffset");
    uint64_t type_entry_array_stride = deref_symbol("gHotSpotVMTypeEntryArrayStride");

    for (;; entry_addr += type_entry_array_stride)
    {
        std::string_view type_name = get_string_view_ref(entry_addr + type_entry_type_name_offset);
        if (type_name.empty())
            break;

        std::string_view superclass_name = get_string_view_ref(entry_addr + type_entry_superclass_name_offset);
        bool is_oop_type = *(int *)(entry_addr + type_entry_is_oop_type_offset);
        bool is_integer_type = *(int *)(entry_addr + type_entry_is_integer_type_offset);
        bool is_unsigned = *(int *)(entry_addr + type_entry_is_unsigned_offset);
        uint64_t size = *(uint64_t *)(entry_addr + type_entry_size_Offset);

        auto type = std::make_unique<types::Type>(type_name,
                                                  lookup_type_or_create_type(superclass_name, -1, false, false, false),
                                                  size, is_oop_type, is_integer_type, is_unsigned);
        std::string_view key = type->get_name();
        name_to_type.try_emplace(key, std::move(type));
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

    name_to_type.try_emplace("<opaque>", std::make_unique<types::Type>("<opaque>", nullptr, -1, false, false, false));

    for (;; entry_addr += struct_entry_array_stride)
    {
        std::string_view field_name = get_string_view_ref(entry_addr + struct_entry_field_name_offset);
        std::string_view type_name = get_string_view_ref(entry_addr + struct_entry_type_name_offset);
        if (field_name.empty())
            break;

        std::string_view type_string = get_string_view_ref(entry_addr + struct_entry_type_string_offset);
        if (type_string.empty())
            type_string = "<opaque>";
        bool is_static = *(int *)(entry_addr + struct_entry_is_static_offset);
        uint64_t offset =
            *(uint64_t *)(entry_addr + (is_static ? struct_entry_address_offset : struct_entry_offset_offset));

        auto type = lookup_type(type_name);
        auto field_type = lookup_type(type_string);
        if (type)
            type->add_field(std::make_unique<types::Field>(type_name, field_name, field_type, is_static, offset));
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
        std::string_view name = get_string_view_ref(entry_addr + int_constant_entry_name_offset);
        if (name.empty())
            break;
        int32_t value = *(int32_t *)(entry_addr + int_constant_entry_value_offset);

        auto old_value = lookup_int_constant(name);
        if (!old_value)
            name_to_int_constant.try_emplace(std::move(name), value);
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
        std::string_view name = get_string_view_ref(entry_addr + long_constant_entry_name_offset);
        if (name.empty())
            break;
        int64_t value = *(int64_t *)(entry_addr + long_constant_entry_value_offset);

        auto old_value = lookup_long_constant(name);
        if (!old_value)
            name_to_long_constant.try_emplace(std::move(name), value);
    }
}

types::Type *Jvm::createBasicType(std::string_view type_name, size_t size, bool is_oop_type, bool is_integer_type,
                                  bool is_unsigned)
{
    std::unique_ptr<types::Type> new_type;

    if (is_integer_type || !is_pointer_type(type_name))
    {
        new_type = std::make_unique<types::Type>(type_name, nullptr, size, is_oop_type, is_integer_type, is_unsigned);
        types::Type *type = new_type.get();
        return name_to_type.try_emplace(type->get_name(), std::move(new_type)).first->second.get();
    }
    return recursive_create_pointer_type(type_name);
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
    auto result_type = basic_lookup_type(type_name);
    if (result_type)
        return result_type;
    auto star_pos = type_name.find_last_of('*');
    if (star_pos == std::string_view::npos)
        return nullptr;
    auto target_type_name = trim(type_name.substr(0, star_pos));
    types::Type *target_type;
    if (is_pointer_type(target_type_name))
    {
        target_type = lookup_type(target_type_name);
        if (!target_type)
            target_type = recursive_create_pointer_type(target_type_name);
    }
    else
    {
        target_type = lookup_type(target_type_name);
        if (!target_type)
        {
            if (target_type_name == "char" || target_type_name == "const char")
                target_type = createBasicType(target_type_name, sizeof(char), false, true, false);
            else if (target_type_name == "u_char")
                target_type = createBasicType(target_type_name, sizeof(unsigned char), false, true, true);
            else
                target_type = createBasicType(target_type_name, -1, false, false, false);
        }
    }
    auto new_type =
        std::make_unique<types::Type>(type_name, nullptr, sizeof(void *), false, false, false, true, target_type);
    result_type = new_type.get();
    name_to_type.try_emplace(result_type->get_name(), std::move(new_type));
    return result_type;
}

bool Jvm::is_pointer_type(std::string_view type_name) noexcept
{
    auto it = type_name.rbegin();
    while (it != type_name.rend() && std::isspace((unsigned char)*it))
        ++it;
    return it != type_name.rend() && *it == '*';
}

#ifdef _WIN32
HMODULE Jvm::get_jvm_handle() noexcept
{
    static HMODULE h = []() { return GetModuleHandleA("jvm.dll"); }();
    return h;
}
#endif

std::string Jvm::vtbl_symbol_for_type(types::Type *type)
{
#ifdef _WIN32
    return std::format("??_7{}@@6B@", type->get_name());
#else
    std::string_view vt{"_ZTV"};
    static bool use_legacy_vt = [] { return lookup_by_name("_vt_10JavaThread"); }();
    if (use_legacy_vt)
        vt = "_vt_";
    return std::format("{}{}{}", vt, type->get_name().size(), type->get_name());
#endif
}
} // namespace hotspot::runtime