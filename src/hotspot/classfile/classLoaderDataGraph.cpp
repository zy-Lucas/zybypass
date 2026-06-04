#include "classLoaderDataGraph.hpp"

namespace hotspot::classfile
{
oops::Klass ClassLoaderDataGraph::find(std::string_view class_name) noexcept
{
    for (ClassLoaderData cld{head()}; cld; cld = cld.next())
        if (oops::Klass k{cld.find(class_name)}; k)
            return k;
    return 0;
}

void ClassLoaderDataGraph::initialize()
{
    utils::FieldResolver r{"ClassLoaderDataGraph"};

    r.field_offset("_head", head_offset_);
}
} // namespace hotspot::classfile