#include "nmethodBucket.hpp"

namespace hotspot::code
{
void nmethodBucket::initialize()
{
    nmethod_offset_ = 0;
    count_offset_ = nmethod_offset_ + sizeof(void *);
    next_offset_ = count_offset_ + sizeof(void *);
    purge_list_next_offset_ = next_offset_ + sizeof(void *);
}
} // namespace hotspot::code