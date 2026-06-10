#pragma once

#include "classLoaderData.hpp"

namespace hotspot::classfile
{
class ClassLoaderDataGraph
{
  public:
    static ClassLoaderData head() noexcept { return runtime::Jvm::atomic_load<uint64_t>(head_offset_); }

    static oops::Klass find(std::string_view class_name) noexcept;

    static void classes_do(auto &&visitor);

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t head_offset_;
};

void ClassLoaderDataGraph::classes_do(auto &&visitor)
{
    for (ClassLoaderData cld{head()}; cld; cld = cld.next())
        cld.classes_do(visitor);
}
} // namespace hotspot::classfile