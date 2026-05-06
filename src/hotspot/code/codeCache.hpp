#pragma once

#include "../memory/codeHeap.hpp"
#include "../runtime/jvmObject.hpp"
#include "../utilities/growableArray.hpp"
#include <thread>

namespace hotspot::code
{
class CodeCache
{
  public:
    CodeCache() = delete;

    static bool contains(uint64_t p) noexcept;

    static void iterator_nmethods(auto &&visitor);

  private:
    DECLARE_STATIC_INIT

    static inline utilities::GrowableArray<memory::CodeHeap> heaps{0};
};

void CodeCache::iterator_nmethods(auto &&visitor)
{
    const int32_t heap_length = heaps.length();

    if (std::thread::hardware_concurrency() <= heap_length)
    {
        for (int i = 0; i < heap_length; ++i)
            if (auto h = heaps.at(i); h.get_code_blob_type() < 2)
            {
                pthread_jit_write_protect_np(0);
                h.iterate(visitor);
                pthread_jit_write_protect_np(1);
            }
    }
    else
    {
        std::vector<std::thread> threads;
        threads.reserve(heap_length);

        for (int i = 0; i < heap_length; ++i)
            if (auto h = heaps.at(i); h.get_code_blob_type() < 2)
                threads.emplace_back([&, h]() mutable {
                    pthread_jit_write_protect_np(0);
                    h.iterate(visitor);
                    pthread_jit_write_protect_np(1);
                });
        for (auto &t : threads)
            t.join();
    }
}
} // namespace hotspot::code