#include "classLoaderDataGraph.hpp"

namespace hotspot::classfile
{
oops::Klass ClassLoaderDataGraph::find(std::string_view class_name) noexcept
{
    for (ClassLoaderData cld = get_head(); cld; cld = cld.next())
        if (oops::Klass k = cld.find(class_name); k)
            return k;
    return 0;
}

void ClassLoaderDataGraph::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("ClassLoaderDataGraph");

    head_offset = *type->get_field_offset("_head");
}
} // namespace hotspot::classfile