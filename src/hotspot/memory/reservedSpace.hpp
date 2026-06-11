#pragma once

#include <cstdint>

namespace hotspot::memory
{
class ReservedSpace
{
  public:
    int8_t *base() const noexcept { return base_; }
    uint64_t size() const noexcept { return size_; }
    int8_t *end() const noexcept { return base_ + size_; }
    uint64_t alignment() const noexcept { return alignment_; }
    uint64_t page_size() const noexcept { return page_size_; }
    bool special() const noexcept { return special_; }
    bool executable() const noexcept { return executable_; }
    uint64_t noaccess_prefix() const noexcept { return noaccess_prefix_; }
    bool is_reserved() const noexcept { return base_; }

  protected:
    int8_t *base_;
    uint64_t size_;
    uint64_t noaccess_prefix_;
    uint64_t alignment_;
    uint64_t page_size_;
    bool special_;
    int32_t fd_for_heap_;

  private:
    ReservedSpace(int8_t *base, uint64_t size, uint64_t alignment, uint64_t page_size, bool special,
                  bool executable) noexcept;

    void initialize_members(int8_t *base, uint64_t size, uint64_t alignment, uint64_t page_size, bool special,
                            bool executable) noexcept;
    void initialize(uint64_t size, uint64_t alignment, uint64_t page_size, int8_t *requested_address,
                    bool executable) noexcept;

    void reserve(uint64_t size, uint64_t alignment, uint64_t page_size, int8_t *requested_address,
                 bool executable) noexcept;

    bool executable_;
};
} // namespace hotspot::memory