#include "metaData.hpp"
#include "../runtime/virtualBaseConstructor.hpp"
#include "constMethod.hpp"
#include "constantPool.hpp"
#include "constantPoolCache.hpp"
#include "instanceKlass.hpp"
#include "klass.hpp"
#include "method.hpp"
#include "objArrayKlass.hpp"
#include "typeArrayKlass.hpp"

namespace hotspot::oops
{
using MetaDataConstructor = runtime::VirtualBaseConstructor<
    std::nullopt_t, runtime::TypeMapping<"Metadata", MetaData>, runtime::TypeMapping<"Klass", Klass>,
    runtime::TypeMapping<"InstanceKlass", InstanceKlass>, runtime::TypeMapping<"TypeArrayKlass", TypeArrayKlass>,
    runtime::TypeMapping<"ObjArrayKlass", ObjArrayKlass>, runtime::TypeMapping<"Method", Method>,
    runtime::TypeMapping<"ConstMethod", ConstMethod>, runtime::TypeMapping<"ConstantPool", ConstantPool>,
    runtime::TypeMapping<"ConstantPoolCache", ConstantPoolCache>>;

void MetaData::initialize() {}

std::pair<std::string_view, runtime::JvmObjectPtr> MetaData::instantiate_wrapper_for(uint64_t addr)
{
    static MetaDataConstructor ctor{runtime::Jvm::lookup_type("Metadata")};
    return ctor.instantiate_wrapper_for(addr);
}
} // namespace hotspot::oops