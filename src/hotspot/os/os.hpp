#pragma once

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
}