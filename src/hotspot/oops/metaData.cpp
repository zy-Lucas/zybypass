#include "metaData.hpp"
#include "../runtime/virtualBaseConstructor.hpp"
#include "constMethod.hpp"
#include "constantPool.hpp"
#include "klass.hpp"
#include "method.hpp"
#include <optional>

namespace hotspot::oops
{
using MetaDataConstructor =
    runtime::VirtualBaseConstructor<std::nullopt_t, runtime::TypeMapping<"Metadata", MetaData>,
                                    runtime::TypeMapping<"Klass", Klass>, runtime::TypeMapping<"Method", Method>,
                                    runtime::TypeMapping<"ConstMethod", ConstMethod>,
                                    runtime::TypeMapping<"ConstantPool", ConstantPool>>;

MetaData::MetaData(uint64_t addr) : runtime::JvmObject<MetaData>(addr) {}

void MetaData::initialize() {}

std::pair<std::string_view, std::unique_ptr<runtime::JvmObjectBase>> MetaData::instantiate_wrapper_for(uint64_t addr)
{
    static std::unique_ptr<runtime::InstanceConstructor> ctor =
        std::make_unique<MetaDataConstructor>(runtime::Jvm::lookup_type("Metadata"));
    return ctor->instantiate_wrapper_for(addr);
}
} // namespace hotspot::oops