#include "universe.hpp"
#include "../gc/epsilon/epsilonHeap.hpp"
#include "../gc/g1/g1CollectedHeap.hpp"
#include "../gc/parallel/parallelScavengeHeap.hpp"
#include "../runtime/virtualConstructor.hpp"

namespace hotspot::memory
{
using UniverseConstructor =
    runtime::VirtualConstructor<runtime::TypeMapping<"ParallelScavengeHeap", gc::parallel::ParallelScavengeHeap>,
                                runtime::TypeMapping<"G1CollectedHeap", gc::g1::G1CollectedHeap>,
                                runtime::TypeMapping<"EpsilonHeap", gc::epsilon::EpsilonHeap>>;

std::unique_ptr<gc::shared::CollectedHeap> Universe::heap()
{
    static UniverseConstructor heap_constructor{};
    return std::unique_ptr<gc::shared::CollectedHeap>{static_cast<gc::shared::CollectedHeap *>(
        heap_constructor.instantiate_wrapper_for(runtime::Jvm::read<uint64_t>(collected_heap_offset))
            .second.release())};
}

void Universe::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Universe");

    collected_heap_offset = *type->get_field_offset("_collectedHeap");
}
} // namespace hotspot::memory