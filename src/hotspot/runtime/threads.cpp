#include "threads.hpp"
#include "javaThread.hpp"
#include "jvm.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <mach/mach.h>
#endif

namespace hotspot::runtime
{
uint64_t ThreadsList::java_thread_address_at(uint32_t i) const noexcept
{
    return Jvm::read<uint64_t>(read_field<uint64_t>(threads_offset_) + i * sizeof(void *));
}

void ThreadsList::initialize()
{
    types::Type *type = Jvm::lookup_type("ThreadsList");

    length_offset_ = *type->field_offset("_length");
    threads_offset_ = *type->field_offset("_threads");
}

JavaThread Threads::current() noexcept
{
    list_ = Jvm::read<uint64_t>(thread_list_offset_);
    for (uint32_t i = 0; i < list_.length(); ++i)
    {
        JavaThread thread{list_.java_thread_address_at(i)};
        if (!thread)
            continue;
#ifdef _WIN32
        if (thread.osthread().thread_id() == GetCurrentThreadId())
            return thread;
#else
        if (thread.osthread().thread_id() == mach_thread_self())
            return thread;
#endif
    }
    return 0;
}

void Threads::initialize()
{
    types::Type *type = Jvm::lookup_type("ThreadsSMRSupport");

    thread_list_offset_ = *type->field_offset("_java_thread_list");
}
} // namespace hotspot::runtime