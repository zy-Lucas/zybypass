#pragma once

#include "metaspace/metaspaceArena.hpp"

namespace hotspot::memory
{
class ClassLoaderMetaspace : public runtime::JvmObject
{
  public:
    ClassLoaderMetaspace(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    metaspace::MetaspaceArena non_class_space_arena() const noexcept;
    metaspace::MetaspaceArena class_space_arena() const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t lock_offset_;
    static inline uint64_t space_type_offset_;
    static inline uint64_t non_class_space_arena_offset_;
    static inline uint64_t class_space_arena_offset_;
};
} // namespace hotspot::memory