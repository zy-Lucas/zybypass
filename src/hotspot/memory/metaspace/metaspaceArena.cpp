#include "metaspaceArena.hpp"
#include "memory/metaspace/freeBlocks.hpp"
#include "runtime/mutexLocker.hpp"

namespace hotspot::memory::metaspace
{
uint64_t *MetaspaceArena::allocate(uint64_t requested_word_size)
{
    runtime::MutexLocker ml(lock(), false);

    uint64_t *p = nullptr;
    uint64_t raw_word_size = get_raw_word_size_for_requested_word_size(requested_word_size);

    if (!fbl() || fbl().is_empty())
        return nullptr;

    p = fbl().remove_block(raw_word_size);
    if (p)
        return p;

    bool current_chunk_too_small = false;
    bool commit_failure = false;

    //     if (current_chunk() != NULL)
    //     {
    //         if (current_chunk()->free_words() < raw_word_size)
    //         {
    //             if (!attempt_enlarge_current_chunk(raw_word_size))
    //             {
    //                 current_chunk_too_small = true;
    //             }
    //             else
    //             {
    //                 DEBUG_ONLY(InternalStats::inc_num_chunks_enlarged();)
    //                 UL(debug, "enlarged chunk.");
    //             }
    //         }

    //         if (!current_chunk_too_small)
    //         {
    //             if (!current_chunk()->ensure_committed_additional(raw_word_size))
    //             {
    //                 UL2(info, "commit failure (requested size: " SIZE_FORMAT ")", raw_word_size);
    //                 commit_failure = true;
    //             }
    //         }

    //         if (!current_chunk_too_small && !commit_failure)
    //         {
    //             p = current_chunk()->allocate(raw_word_size);
    //             assert(p != NULL, "Allocation from chunk failed.");
    //         }
    //     }

    //     if (p == NULL)
    //     {
    //         // If we are here, we either had no current chunk to begin with or it was deemed insufficient.
    //         assert(current_chunk() == NULL || current_chunk_too_small || commit_failure, "Sanity");

    //         Metachunk *new_chunk = allocate_new_chunk(raw_word_size);
    //         if (new_chunk != NULL)
    //         {
    //             UL2(debug, "allocated new chunk " METACHUNK_FORMAT " for requested word size " SIZE_FORMAT ".",
    //                 METACHUNK_FORMAT_ARGS(new_chunk), requested_word_size);

    //             assert(new_chunk->free_below_committed_words() >= raw_word_size, "Sanity");
    //             if (Settings::new_chunks_are_fully_committed())
    //             {
    //                 assert(new_chunk->is_fully_committed(), "Chunk should be fully committed.");
    //             }

    //             // We have a new chunk. Before making it the current chunk, retire the old one.
    //             if (current_chunk() != NULL)
    //             {
    //                 salvage_chunk(current_chunk());
    //                 DEBUG_ONLY(InternalStats::inc_num_chunks_retired();)
    //             }

    //             _chunks.add(new_chunk);

    //             // Now, allocate from that chunk. That should work.
    //             p = current_chunk()->allocate(raw_word_size);
    //             assert(p != NULL, "Allocation from chunk failed.");
    //         }
    //         else
    //         {
    //             UL2(info, "failed to allocate new chunk for requested word size " SIZE_FORMAT ".",
    //             requested_word_size);
    //         }
    //     }

    //     if (p == NULL)
    //     {
    //         InternalStats::inc_num_allocs_failed_limit();
    //     }
    //     else
    //     {
    //         DEBUG_ONLY(InternalStats::inc_num_allocs();)
    //         _total_used_words_counter->increment_by(raw_word_size);
    //     }

    //     SOMETIMES(verify_locked();)

    //     if (p == NULL)
    //     {
    //         UL(info, "allocation failed, returned NULL.");
    //     }
    //     else
    //     {
    //         UL2(trace, "after allocation: %u chunk(s), current:" METACHUNK_FULL_FORMAT, _chunks.count(),
    //             METACHUNK_FULL_FORMAT_ARGS(current_chunk()));
    //         UL2(trace, "returning " PTR_FORMAT ".", p2i(p));
    //     }
    return p;
}

uint64_t MetaspaceArena::get_raw_word_size_for_requested_word_size(uint64_t word_size) noexcept
{
    uint64_t bytes = std::max(word_size, BinList32::MinWordSize) * runtime::Jvm::bytes_per_word();
    return runtime::Jvm::align_up(bytes, 8) / runtime::Jvm::bytes_per_word();
}

void MetaspaceArena::initialize()
{
    lock_offset_ = 0;
    chunk_manager_offset_ = lock_offset_ + sizeof(void *);
    growth_policy_offset_ = chunk_manager_offset_ + sizeof(void *);
    chunks_offset_ = growth_policy_offset_ + sizeof(void *);
    fbl_offset_ = chunks_offset_ + 2 * sizeof(void *);
    total_used_words_counter_offset_ = fbl_offset_ + sizeof(void *);
    name_offset_ = total_used_words_counter_offset_ + sizeof(void *);
}
} // namespace hotspot::memory::metaspace