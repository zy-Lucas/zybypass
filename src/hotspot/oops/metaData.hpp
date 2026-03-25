#pragma once

#include "../runtime/jvmObject.hpp"

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
} // namespace hotspot::oops