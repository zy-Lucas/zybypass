#include "classLoaderMetaspace.hpp"

namespace hotspot::memory
{
metaspace::MetaspaceArena ClassLoaderMetaspace::non_class_space_arena() const noexcept
{
    return read_field<uint64_t>(non_class_space_arena_offset_);
}

metaspace::MetaspaceArena ClassLoaderMetaspace::class_space_arena() const noexcept
{
    return read_field<uint64_t>(class_space_arena_offset_);
}

void ClassLoaderMetaspace::initialize()
{
    lock_offset_ = 0;
    space_type_offset_ = lock_offset_ + sizeof(void *);
    non_class_space_arena_offset_ = space_type_offset_ + sizeof(void *);
    class_space_arena_offset_ = non_class_space_arena_offset_ + sizeof(void *);
}
} // namespace hotspot::memory