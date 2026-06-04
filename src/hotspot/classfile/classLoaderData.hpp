#pragma once

#include "oops/klass.hpp"

namespace hotspot::classfile
{
class ClassLoaderData : public runtime::JvmObject
{
  public:
    ClassLoaderData(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    bool has_class_mirror_holder() const noexcept { return read_field<bool>(has_class_mirror_holder_offset_); }

    ClassLoaderData next() const noexcept { return read_field<uint64_t>(next_offset_); }

    oops::Klass klasses() const noexcept { return read_field<uint64_t>(klasses_offset_); }

    oops::Klass find(std::string_view class_name) const noexcept;

    void classes_do(auto &&visitor);

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t class_loader_offset_;
    static inline uint64_t has_class_mirror_holder_offset_;
    static inline uint64_t klasses_offset_;
    static inline uint64_t dictionary_offset_;
    static inline uint64_t next_offset_;
};

void ClassLoaderData::classes_do(auto &&visitor)
{
    for (oops::Klass k{klasses()}; k; k = k.next_link())
        visitor(k);
}
} // namespace hotspot::classfile