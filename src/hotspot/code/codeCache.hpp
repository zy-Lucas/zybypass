#pragma once

#include "../memory/codeHeap.hpp"
#include "../runtime/jvmObject.hpp"
#include "../utilities/growableArray.hpp"
#include <thread>

namespace hotspot::code
{
enum
{
    MethodNonProfiled = 0,
    MethodProfiled = 1,
    NonNMethod = 2,
    All = 3,
    NumTypes = 4
};

class CodeCache
{
  public:
    CodeCache() = delete;

    static bool contains(uint64_t p) noexcept;

    static void iterator_nmethods(auto &&visitor);

  private:
    DECLARE_STATIC_INIT

    static inline utilities::GrowableArray<memory::CodeHeap> heaps_{0};
};

void CodeCache::iterator_nmethods(auto &&visitor)
{
    static int32_t heap_length = heaps_.length();
    static bool segmented_mode = heap_length != 1;

    if (std::thread::hardware_concurrency() <= heap_length || !segmented_mode)
        for (uint32_t i = 0; i < heap_length; ++i)
        {
            auto h{heaps_.at(i)};
            auto type = h.code_blob_type();

            if (segmented_mode)
            {
                if (type < NonNMethod)
                    h.iterate(visitor);
            }
            else
                h.iterate([&visitor](code::CodeBlob cb) {
                    if (cb.header_size() == 344/*sizeof(nmethod)*/)
                        visitor(cb);
                });
        }
    else
    {
        std::vector<std::thread> threads;
        threads.reserve(heap_length);

        for (uint32_t i = 0; i < heap_length; ++i)
            if (auto h{heaps_.at(i)}; h.code_blob_type() < NonNMethod)
                threads.emplace_back([&, h]() mutable { h.iterate(visitor); });
        for (auto &t : threads)
            t.join();
    }
}
} // namespace hotspot::code