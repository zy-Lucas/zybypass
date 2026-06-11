#ifdef _WIN32
#include "os_windows.hpp"
#include "os/os.hpp"

namespace hotspot::os
{
int32_t PlatformMonitor::wait(int64_t millis) noexcept
{
    if (SleepConditionVariableCS(&_cond, &_mutex, millis == 0 ? INFINITE : millis))
        return OS_OK;
    return OS_TIMEOUT;
}
} // namespace hotspot::os
#endif