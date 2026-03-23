#include "symbol_lookup.h"
#include "proc_impl.h"
#include "symtab.h"
#import <Foundation/Foundation.h>
#include <cstring>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <memory>
#include <mutex>
#import <objc/objc-runtime.h>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

namespace
{
using namespace hotspot::debugger::aarch64;

struct symbol_info
{
    uint64_t addr;
};

struct SymbolHash
{
    using is_transparent = void;

    size_t operator()(const std::string &s) const noexcept { return std::hash<std::string>{}(s); }
    size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
};

std::unordered_map<std::string, symbol_info, SymbolHash, std::equal_to<>> g_symbol_map;
std::once_flag g_init_flag;

std::unordered_map<std::string, uint64_t> g_jrs_cache;
std::mutex g_jrs_mutex;

bool build_symtab_for_lib(const lib_info *lib)
{
    int fd = open(lib->name, O_RDONLY);
    if (fd < 0)
        return false;

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        close(fd);
        return false;
    }

    void *mapped = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED)
    {
        close(fd);
        return false;
    }

    uint8_t *data = static_cast<uint8_t *>(mapped);
    size_t file_size = st.st_size;
    size_t img_offset = 0;

    uint32_t magic = *reinterpret_cast<uint32_t *>(data);
    if (magic == FAT_MAGIC || magic == FAT_CIGAM)
    {
        auto *fat_hdr = reinterpret_cast<struct fat_header *>(data);
        uint32_t nfat = ntohl(fat_hdr->nfat_arch);
        auto *archs = reinterpret_cast<struct fat_arch *>(data + sizeof(fat_header));

#if defined(__x86_64__)
        cpu_type_t target_cpu = CPU_TYPE_X86_64;
#else
        cpu_type_t target_cpu = CPU_TYPE_ARM64;
#endif

        for (uint32_t i = 0; i < nfat; i++)
        {
            if (ntohl(archs[i].cputype) == target_cpu)
            {
                img_offset = ntohl(archs[i].offset);
                break;
            }
        }
    }

    if (img_offset >= file_size)
    {
        munmap(mapped, file_size);
        close(fd);
        return false;
    }

    auto *header = reinterpret_cast<mach_header_64 *>(data + img_offset);
    if (header->magic != MH_MAGIC_64 && header->magic != MH_CIGAM_64)
    {
        munmap(mapped, file_size);
        close(fd);
        return false;
    }

    struct symtab_command *sym_cmd = nullptr;
    uint8_t *cmd_ptr = data + img_offset + sizeof(mach_header_64);
    for (uint32_t i = 0; i < header->ncmds; i++)
    {
        auto *cmd = reinterpret_cast<load_command *>(cmd_ptr);
        if (cmd->cmd == LC_SYMTAB)
        {
            sym_cmd = reinterpret_cast<struct symtab_command *>(cmd);
            break;
        }
        cmd_ptr += cmd->cmdsize;
    }

    if (!sym_cmd || sym_cmd->stroff == 0 || sym_cmd->symoff == 0)
    {
        munmap(mapped, file_size);
        close(fd);
        return false;
    }

    auto *sym_table = reinterpret_cast<nlist_64 *>(data + img_offset + sym_cmd->symoff);
    const char *str_table = reinterpret_cast<const char *>(data + img_offset + sym_cmd->stroff);

    for (uint32_t i = 0; i < sym_cmd->nsyms; i++)
    {
        auto &sym = sym_table[i];

        if (sym.n_un.n_strx == 0 || sym.n_value == 0)
            continue;
        if ((sym.n_type & N_STAB) != 0)
            continue;
        if ((sym.n_type & N_TYPE) == N_INDR)
            continue;

        const char *sym_name = str_table + sym.n_un.n_strx;
        if (!sym_name[0])
            continue;

        uint64_t final_addr = lib->base + sym.n_value;

        // 插入不带下划线的版本（如 "_malloc" -> "malloc"）
        if (sym_name[0] == '_')
        {
            g_symbol_map.try_emplace(sym_name + 1, final_addr);
        }
        // 保留原始名称
        g_symbol_map.try_emplace(sym_name, final_addr);
    }

    munmap(mapped, file_size);
    close(fd);
    return true;
}

void initialize_libs()
{
    uint32_t count = _dyld_image_count();
    std::vector<std::unique_ptr<lib_info>> libs;
    libs.reserve(count);

    for (uint32_t i = 0; i < count; i++)
    {
        const char *name = _dyld_get_image_name(i);
        const auto *header = _dyld_get_image_header(i);
        if (!name || !header)
            continue;

        auto lib = std::make_unique<lib_info>();
        std::strncpy(lib->name, name, sizeof(lib->name) - 1);
        lib->name[sizeof(lib->name) - 1] = '\0';
        lib->base = reinterpret_cast<uintptr_t>(header);
        libs.push_back(std::move(lib));
    }

    g_symbol_map.reserve(libs.size() * 2000);

    for (auto &lib : libs)
    {
        build_symtab_for_lib(lib.get());
    }
}

uint64_t lookup_by_name_jrs_cached(const char *symbolName)
{
    {
        std::lock_guard<std::mutex> lock(g_jrs_mutex);
        auto it = g_jrs_cache.find(symbolName);
        if (it != g_jrs_cache.end())
            return it->second;
    }

    uint64_t address = 0;
    @autoreleasepool
    {
        NSString *sym = @(symbolName);
        if (!sym)
            return 0;

        Class jrs = objc_lookUpClass("JRSSymbolicator");
        if (!jrs)
            return 0;

        id symbolicator = ((id(*)(id, SEL, pid_t))objc_msgSend)(jrs, @selector(symbolicatorForPid:), getpid());
        if (!symbolicator)
            return 0;

        address = (uint64_t)((uint64_t (*)(id, SEL, NSString *))objc_msgSend)(symbolicator,
                                                                              @selector(addressForSymbol:), sym);
    }

    std::lock_guard<std::mutex> lock(g_jrs_mutex);
    g_jrs_cache.emplace(symbolName, address);
    return address;
}
} // namespace

uint64_t lookup_by_name(const char *symbolName)
{
    if (__builtin_expect(!symbolName || !symbolName[0], 0))
        return 0;

    std::call_once(g_init_flag, initialize_libs);

    auto it = g_symbol_map.find(std::string_view(symbolName));
    if (__builtin_expect(it != g_symbol_map.end(), 1))
    {
        return it->second.addr;
    }

    return lookup_by_name_jrs_cached(symbolName);
}