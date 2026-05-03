#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::memory
{
struct Word
{
    uint64_t size;
};

class MemRegion
{
  public:
    MemRegion() = default;

    MemRegion(uint64_t addr);
    MemRegion(uint64_t start, Word word_size) noexcept : _start(start) { set_word_size(word_size.size); }
    MemRegion(uint64_t start, uint64_t limit) noexcept : _start(start), byte_size(limit - start) {}

    uint64_t start() const noexcept { return _start; }
    uint64_t end() const noexcept { return _start + byte_size; }

    bool contains(const MemRegion &mr) const noexcept { return _start <= mr._start && end() >= mr.end(); }
    bool contains(uint64_t addr) const noexcept { return addr >= _start && addr < end(); }

    MemRegion intersection(const MemRegion &mr) const noexcept;

    MemRegion merge(const MemRegion &mr) const noexcept;

    void set_start(uint64_t start) noexcept { _start = start; }
    void set_end(uint64_t end) noexcept { byte_size = end - _start; }
    void set_word_size(uint64_t word_size) noexcept { byte_size = sizeof(void *) * word_size; }

  private:
    uint64_t _start;
    uint64_t byte_size;

    DECLARE_STATIC_INIT

    static inline uint64_t start_offset;
    static inline uint64_t word_size_offset;
};
} // namespace hotspot::memory