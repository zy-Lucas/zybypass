#pragma once

#include "../oops/klass.hpp"
#include "../runtime/jvmObject.hpp"

namespace hotspot::classfile
{
class ClassLoaderData : public runtime::JvmObject
{
  public:
    ClassLoaderData(uint64_t addr) : runtime::JvmObject(addr) {}

    bool has_class_mirror_holder() const noexcept { return read_field<bool>(has_class_mirror_holder_offset); }

    ClassLoaderData next() const noexcept { return read_field<uint64_t>(next_offset); }

    oops::Klass get_klasses() const noexcept { return read_field<uint64_t>(klasses_offset); }

    oops::Klass find(std::string_view class_name) const noexcept;

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t class_loader_offset;
    static inline uint64_t has_class_mirror_holder_offset;
    static inline uint64_t klasses_offset;
    static inline uint64_t dictionary_offset;
    static inline uint64_t next_offset;
};
} // namespace hotspot::classfile