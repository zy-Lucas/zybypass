#pragma once

#include "arrayKlass.hpp"

namespace hotspot::oops
{
class ObjArrayKlass : public ArrayKlass
{
  public:
    ObjArrayKlass(uint64_t addr) : ArrayKlass(addr) {}
    
    Klass get_element_klass() const noexcept { return read_field<uint64_t>(element_Klass_offset); }
    Klass get_bottom_Klass() const noexcept { return read_field<uint64_t>(bottom_klass_offset); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t element_Klass_offset;
    static inline uint64_t bottom_klass_offset;
};
} // namespace hotspot::oops