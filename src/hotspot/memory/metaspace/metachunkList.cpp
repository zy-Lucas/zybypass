#include "metachunkList.hpp"

namespace hotspot::memory::metaspace
{
void MetachunkList::initialize()
{
    first_offset_ = 0;
    num_chunks_offset_ = first_offset_ + sizeof(void *);
}
} // namespace hotspot::memory::metaspace