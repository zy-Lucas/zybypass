#pragma once

#include "oops/instanceKlass.hpp"
#include <jni.h>
#include <string>

class JavaTools
{
  public:
    static std::string to_std_string(jstring string) noexcept;

    static void set_method_to_native(jstring klass_name, jstring method_name, jstring method_sign,
                                     uint64_t native_addr) noexcept;

    static void replace_method(hotspot::oops::InstanceKlass ik, hotspot::oops::Method old_method, int32_t index,
                               hotspot::oops::Method new_method) noexcept;

    static void deoptimization_method(hotspot::oops::Method method) noexcept;
};