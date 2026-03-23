#pragma once

#include "../runtime/jvmObject.hpp"
#include "../runtime/virtualBaseConstructor.hpp"

namespace hotspot::oops
{
class MetaData : public runtime::JvmObject<MetaData>
{
  public:
    MetaData(uint64_t addr);

    static std::pair<std::string_view, std::unique_ptr<runtime::JvmObjectBase>> instantiate_wrapper_for(uint64_t addr);

  private:
    friend class runtime::JvmObject<MetaData>;

    static inline std::once_flag init_flag_;

    static void initialize();
};

using MetadataConstructor =
    runtime::VirtualBaseConstructor<runtime::NullType, runtime::TypeMapping<"Metadata", MetaData>,
                                    runtime::TypeMapping<"Klass", MetaData>>;
} // namespace hotspot::oops