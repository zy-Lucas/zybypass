#include "metaData.hpp"
#include "../runtime/virtualBaseConstructor.hpp"
#include "constMethod.hpp"
#include "constantPool.hpp"
#include "constantPoolCache.hpp"
#include "klass.hpp"
#include "method.hpp"

namespace hotspot::oops
{
using MetaDataConstructor = runtime::VirtualBaseConstructor<
    std::nullopt_t, runtime::TypeMapping<"Metadata", MetaData>, runtime::TypeMapping<"Klass", Klass>,
    runtime::TypeMapping<"Method", Method>, runtime::TypeMapping<"ConstMethod", ConstMethod>,
    runtime::TypeMapping<"ConstantPool", ConstantPool>, runtime::TypeMapping<"ConstantPoolCache", ConstantPoolCache>>;

MetaData::MetaData(uint64_t addr) : runtime::JvmObject(addr) {}

void MetaData::initialize() {}

std::pair<std::string_view, runtime::JvmObject> MetaData::instantiate_wrapper_for(uint64_t addr)
{
    static std::unique_ptr<runtime::InstanceConstructor> ctor =
        std::make_unique<MetaDataConstructor>(runtime::Jvm::lookup_type("Metadata"));
    return ctor->instantiate_wrapper_for(addr);
}
} // namespace hotspot::oops