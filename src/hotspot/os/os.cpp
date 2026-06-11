#include "os.hpp"

namespace hotspot::os
{
void *os::reserve_memory(uint64_t bytes, bool executable, memory::MEMFLAGS flags)
{
    return pd_reserve_memory(bytes, executable);
}

void *os::attempt_reserve_memory_at(void *addr, uint64_t bytes, bool executable)
{
    return pd_attempt_reserve_memory_at(addr, bytes, executable);
}

bool os::commit_memory(void *addr, uint64_t bytes, bool executable)
{
    return pd_commit_memory(addr, bytes, executable);
}

bool os::commit_memory(void *addr, uint64_t size, uint64_t alignment_hint, bool executable)
{
    return pd_commit_memory(addr, size, alignment_hint, executable);
}

void *os::attempt_map_memory_to_file_at(void *base, uint64_t size, int32_t fd)
{
    return pd_attempt_map_memory_to_file_at(base, size, fd);
}

void *os::reserve_memory_special(uint64_t size, uint64_t alignment, uint64_t page_size, void *addr, bool executable)
{
    return pd_reserve_memory_special(size, alignment, page_size, addr, executable);
}
} // namespace hotspot::os