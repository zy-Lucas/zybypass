#pragma once

#include "../runtime/jvmObject.hpp"
#include "classLoaderData.hpp"

namespace hotspot::classfile
{
class ClassLoaderDataGraph
{
  public:
    static ClassLoaderData get_head() noexcept { return runtime::Jvm::read<uint64_t>(head_offset); }

    static oops::Klass find(std::string_view class_name) noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t head_offset;
};
} // namespace hotspot::classfile