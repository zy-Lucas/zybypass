#include "metaData.hpp"

namespace hotspot::oops
{
MetaData::MetaData(uint64_t addr) : runtime::JvmObject<MetaData>(addr) {}

void MetaData::initialize() {}

std::pair<std::string_view, std::unique_ptr<runtime::JvmObjectBase>> MetaData::instantiate_wrapper_for(uint64_t addr)
{
    static std::unique_ptr<runtime::InstanceConstructor> ctor = std::make_unique<MetadataConstructor>(runtime::Jvm::lookup_type("Metadata"));
    return ctor->instantiate_wrapper_for(addr);
}
} // namespace hotspot::oops