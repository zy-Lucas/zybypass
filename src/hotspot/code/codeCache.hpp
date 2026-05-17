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

    static inline utilities::GrowableArray<memory::CodeHeap> heaps{0};
};

void CodeCache::iterator_nmethods(auto &&visitor)
{
    const int32_t heap_length = heaps.length();
    static bool segmented_mode = heap_length != 1;

    if (std::thread::hardware_concurrency() <= heap_length || !segmented_mode)
        for (uint32_t i = 0; i < heap_length; ++i)
        {
            auto h{heaps.at(i)};
            auto type = h.get_code_blob_type();

            if (segmented_mode)
            {
                if (type < NonNMethod)
                    h.iterate(visitor);
            }
            else
                h.iterate([&visitor](code::CodeBlob cb) {
                    if (cb.get_name_view() == "nmethod")
                        visitor(cb);
                });
        }
    else
    {
        std::vector<std::thread> threads;
        threads.reserve(heap_length);

        for (uint32_t i = 0; i < heap_length; ++i)
            if (auto h{heaps.at(i)}; h.get_code_blob_type() < NonNMethod)
                threads.emplace_back([&, h]() mutable { h.iterate(visitor); });
        for (auto &t : threads)
            t.join();
    }
}
} // namespace hotspot::code