#include "thread.hpp"

namespace hotspot::runtime
{
void Thread::initialize()
{
    types::Type *type_thread = Jvm::lookup_type("Thread");
    types::Type *type_java_thread = Jvm::lookup_type("JavaThread");

    tlab_field_offset_ = *type_thread->field_offset("_tlab");
    active_handles_offset_ = *type_thread->field_offset("_active_handles");
    allocated_bytes_offset_ = *type_thread->field_offset("_allocated_bytes");
}
} // namespace hotspot::runtime