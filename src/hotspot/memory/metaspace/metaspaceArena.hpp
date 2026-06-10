#pragma once

#include "memory/metaspace/freeBlocks.hpp"
#include "runtime/mutex.hpp"

namespace hotspot::memory::metaspace
{
class MetaspaceArena : public runtime::JvmObject
{
  public:
    MetaspaceArena(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    runtime::Mutex lock() const noexcept { return read_field<uint64_t>(lock_offset_); }

    FreeBlocks fbl() const noexcept { return read_field<uint64_t>(fbl_offset_); }

    uint64_t *allocate(uint64_t word_size);

    std::string_view name_view() const noexcept { return read_string_field(name_offset_); }
    std::string name() const { return std::string{name_view()}; }

  private:
    static uint64_t get_raw_word_size_for_requested_word_size(uint64_t word_size) noexcept;

    DECLARE_STATIC_INIT

    static inline uint64_t lock_offset_;
    static inline uint64_t chunk_manager_offset_;
    static inline uint64_t growth_policy_offset_;
    static inline uint64_t chunks_offset_;
    static inline uint64_t fbl_offset_;
    static inline uint64_t total_used_words_counter_offset_;
    static inline uint64_t name_offset_;
};
} // namespace hotspot::memory::metaspace