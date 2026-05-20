#pragma once

#include "../runtime/instanceConstructor.hpp"
#include "../runtime/jvmObject.hpp"

namespace hotspot::oops
{
class MetaData : public runtime::JvmObject
{
  public:
    MetaData(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    static std::pair<std::string_view, runtime::JvmObjectPtr> instantiate_wrapper_for(uint64_t addr);

    static uint64_t align_size(uint64_t size)
    {
        return runtime::Jvm::align_up(size, runtime::Jvm::bytes_per_word());
    }

  private:
    DECLARE_STATIC_INIT
};
} // namespace hotspot::oops