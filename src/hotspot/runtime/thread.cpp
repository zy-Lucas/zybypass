#include "thread.hpp"

namespace hotspot::runtime
{
void Thread::initialize()
{
    utils::FieldResolver r{"Thread"};
    
    r.field_offset("_tlab", tlab_field_offset_);
    r.field_offset("_active_handles", active_handles_offset_);
    r.field_offset("_allocated_bytes", allocated_bytes_offset_);
}
} // namespace hotspot::runtime