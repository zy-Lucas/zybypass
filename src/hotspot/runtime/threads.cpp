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
    utils::FieldResolver r{"ThreadsList"};

    r.field_offset("_length", length_offset_);
    r.field_offset("_threads", threads_offset_);
}

JavaThread Threads::current() noexcept
{
    for (uint32_t index = 0; index < list().length(); ++index)
    {
        JavaThread thread{list().java_thread_address_at(index)};
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
    utils::FieldResolver r{"ThreadsSMRSupport"};

    r.field_offset("_java_thread_list", thread_list_offset_);
}
} // namespace hotspot::runtime