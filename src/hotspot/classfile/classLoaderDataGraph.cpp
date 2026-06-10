#include "classLoaderDataGraph.hpp"
#include "runtime/mutex.hpp"

namespace hotspot::classfile
{
oops::Klass ClassLoaderDataGraph::find(std::string_view class_name) noexcept
{
    for (ClassLoaderData cld{head()}; cld; cld = cld.next())
    {
        memory::metaspace::MetaspaceArena space{cld.metapace_or_null().non_class_space_arena()};
        if (space && space.lock() && space.fbl() && space.fbl().small_blocks() && space.fbl().tree())
        {
            std::cout << "address: " << space.allocate(68) << std::endl;
        }
        if (oops::Klass k{cld.find(class_name)}; k)
            return k;
    }
    return 0;
}

void ClassLoaderDataGraph::initialize()
{
    utils::FieldResolver r{"ClassLoaderDataGraph"};

    r.field_offset("_head", head_offset_);
}
} // namespace hotspot::classfile