#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::oops
{
class MetaData : public runtime::JvmObject<MetaData>
{
  public:
    MetaData(uint64_t addr);

    static std::pair<std::string_view, runtime::JvmObjectBase> instantiate_wrapper_for(uint64_t addr);

    static constexpr uint64_t align_size(uint64_t size)
    {
        return runtime::Jvm::align_up(size, *runtime::Jvm::get_bytes_per_word());
    }

  private:
    friend class runtime::JvmObject<MetaData>;

    DECLARE_STATIC_INIT
};
} // namespace hotspot::oops