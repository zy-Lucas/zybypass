#include "universe.hpp"
#include "../gc/epsilon/epsilonHeap.hpp"
#include "../gc/g1/g1CollectedHeap.hpp"
#include "../gc/parallel/parallelScavengeHeap.hpp"
#include "../gc/serial/serialHeap.hpp"
#include "../gc/shenandoah/shenandoahHeap.hpp"
#include "../gc/z/zCollectedHeap.hpp"
#include "../runtime/basicType.hpp"
#include "../runtime/virtualConstructor.hpp"

namespace hotspot::memory
{
using UniverseConstructor =
    runtime::VirtualConstructor<runtime::TypeMapping<"SerialHeap", gc::serial::SerialHeap>,
                                runtime::TypeMapping<"ParallelScavengeHeap", gc::parallel::ParallelScavengeHeap>,
                                runtime::TypeMapping<"G1CollectedHeap", gc::g1::G1CollectedHeap>,
                                runtime::TypeMapping<"EpsilonHeap", gc::epsilon::EpsilonHeap>,
                                runtime::TypeMapping<"ZCollectedHeap", gc::z::ZCollectedHeap>,
                                runtime::TypeMapping<"ShenandoahHeap", gc::shenandoah::ShenandoahHeap>>;

gc::shared::CollectedHeap *Universe::heap()
{
    static std::unique_ptr<gc::shared::CollectedHeap> heap{static_cast<gc::shared::CollectedHeap *>(
        UniverseConstructor{}
            .instantiate_wrapper_for(runtime::Jvm::read<uint64_t>(collected_heap_offset_))
            .second.release())};
    return heap.get();
}

bool Universe::element_type_should_be_aligned(int32_t type) noexcept
{
    return type == runtime::BasicType::T_DOUBLE || type == runtime::BasicType::T_LONG;
}

void Universe::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("Universe");

    collected_heap_offset_ = *type->field_offset("_collectedHeap");
}
} // namespace hotspot::memory