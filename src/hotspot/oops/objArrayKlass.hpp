#pragma once

#include "arrayKlass.hpp"

namespace hotspot::oops
{
class ObjArrayKlass : public ArrayKlass
{
  public:
    ObjArrayKlass(uint64_t addr) noexcept : ArrayKlass(addr) {}
    
    Klass element_klass() const noexcept { return read_field<uint64_t>(element_Klass_offset_); }
    Klass bottom_Klass() const noexcept { return read_field<uint64_t>(bottom_klass_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t element_Klass_offset_;
    static inline uint64_t bottom_klass_offset_;
};
} // namespace hotspot::oops