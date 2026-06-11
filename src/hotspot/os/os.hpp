#pragma once

#include "memory/allocation.hpp"
#include <cstdint>

#ifndef _WIN32
#include "os/posix/os_posix.hpp"
#else
#include "os/windows/os_windows.hpp"
#endif

namespace hotspot::os
{
enum OSReturn
{
    OS_OK = 0,         // Operation was successful
    OS_ERR = -1,       // Operation failed
    OS_INTRPT = -2,    // Operation was interrupted
    OS_TIMEOUT = -3,   // Operation timed out
    OS_NOMEM = -5,     // Operation failed for lack of memory
    OS_NORESOURCE = -6 // Operation failed for lack of nonmemory resource
};

class os
{
  private:
    static void *pd_reserve_memory(uint64_t bytes, bool executable);

    static void *pd_attempt_reserve_memory_at(void *addr, uint64_t bytes, bool executable);

    static bool pd_commit_memory(void *addr, uint64_t bytes, bool executable);
    static bool pd_commit_memory(void *addr, uint64_t size, uint64_t alignment_hint, bool executable);
    static bool pd_release_memory(void *addr, uint64_t bytes);

    static void *pd_attempt_map_memory_to_file_at(void *addr, uint64_t bytes, int file_desc);

    static bool pd_unmap_memory(void *addr, uint64_t bytes);

    static void *pd_reserve_memory_special(uint64_t size, uint64_t alignment, uint64_t page_size, void *addr,
                                           bool executable);
    static bool pd_release_memory_special(void *addr, uint64_t bytes);

  public:
    static int32_t vm_page_size();

    static int32_t vm_allocation_granularity();

    static void *reserve_memory(uint64_t bytes, bool executable = false, memory::MEMFLAGS flags = memory::mtOther);
    static void *reserve_memory_aligned(uint64_t size, uint64_t alignment, bool executable = false);

    static void *attempt_reserve_memory_at(void *addr, uint64_t bytes, bool executable = false);

    static bool commit_memory(void *addr, uint64_t bytes, bool executable);
    static bool commit_memory(void *addr, uint64_t size, uint64_t alignment_hint, bool executable);

    static bool release_memory(void *addr, uint64_t bytes) { return pd_release_memory(addr, bytes); }

    static void *map_memory_to_file(uint64_t size, int32_t fd) { return map_memory_to_file(nullptr, size, fd); }
    static void *map_memory_to_file_aligned(uint64_t size, uint64_t alignment, int32_t fd);
    static void *map_memory_to_file(void *base, uint64_t size, int32_t fd);
    static void *attempt_map_memory_to_file_at(void *base, uint64_t size, int32_t fd);

    static void *reserve_memory_special(uint64_t size, uint64_t alignment, uint64_t page_size, void *addr,
                                        bool executable);
    static bool release_memory_special(void *addr, uint64_t bytes) { return pd_release_memory_special(addr, bytes); }
};
} // namespace hotspot::os