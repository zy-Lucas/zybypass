#pragma once

#include <cstdint>

namespace hotspot::memory::metaspace
{
template <class T> class AbstractCounter
{
  public:
    AbstractCounter() noexcept : c_(0) {}

    T get() const noexcept { return c_; }

    void increment() noexcept { increment_by(1); }
    void decrement() noexcept { decrement_by(1); }

    void increment_by(T v) noexcept { c_ += v; }
    void decrement_by(T v) noexcept { c_ -= v; }

    void reset() noexcept { c_ = 0; }

  private:
    T c_;
};

typedef AbstractCounter<uint64_t> SizeCounter;
typedef AbstractCounter<uint32_t> IntCounter;

template <class T_num, class T_size> class AbstractMemoryRangeCounter
{
  public:
    void add(T_size s) noexcept
    {
        if (s > 0)
        {
            count_.increment();
            total_size_.increment_by(s);
        }
    }

    void sub(T_size s) noexcept
    {
        if (s > 0)
        {
            count_.decrement();
            total_size_.decrement_by(s);
        }
    }

    T_num count() const noexcept { return count_.get(); }
    T_size total_size() const noexcept { return total_size_.get(); }

  private:
    AbstractCounter<T_num> count_;
    AbstractCounter<T_size> total_size_;
};

typedef AbstractMemoryRangeCounter<uint32_t, uint64_t> MemRangeCounter;
} // namespace hotspot::memory::metaspace