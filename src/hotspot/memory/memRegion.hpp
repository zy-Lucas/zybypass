#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::memory
{
struct Word
{
    uint64_t size_;
};

class MemRegion
{
  public:
    MemRegion() = default;

    MemRegion(uint64_t addr) noexcept;
    MemRegion(uint64_t start, Word word_size) noexcept : start_(start) { set_word_size(word_size.size_); }
    MemRegion(uint64_t start, uint64_t limit) noexcept : start_(start), byte_size_(limit - start) {}

    uint64_t start() const noexcept { return start_; }
    void set_start(uint64_t start) noexcept { start_ = start; }

    uint64_t end() const noexcept { return start_ + byte_size_; }
    void set_end(uint64_t end) noexcept { byte_size_ = end - start_; }
    void set_word_size(uint64_t word_size) noexcept { byte_size_ = sizeof(void *) * word_size; }

    bool contains(const MemRegion &mr) const noexcept { return start_ <= mr.start_ && end() >= mr.end(); }
    bool contains(uint64_t addr) const noexcept { return addr >= start_ && addr < end(); }

    MemRegion intersection(const MemRegion &mr) const noexcept;

    MemRegion merge(const MemRegion &mr) const noexcept;

  private:
    uint64_t start_;
    uint64_t byte_size_;

    DECLARE_STATIC_INIT

    static inline uint64_t start_offset_;
    static inline uint64_t word_size_offset_;
};
} // namespace hotspot::memory