#pragma once

#include "oops/instanceKlass.hpp"
#include "oops/method.hpp"
#include <jni.h>
#include <utility>

class JavaTools
{
  public:
    static std::string to_std_string(jstring string) noexcept;

    static std::pair<hotspot::oops::Method, int32_t> lookup_method(jstring klass_name, jstring method_name,
                                                                 jstring method_sign);

    static void set_method_to_native(jstring klass_name, jstring method_name, jstring method_sign,
                                     uint64_t native_addr) noexcept;

    static void replace_method(hotspot::oops::InstanceKlass ik, hotspot::oops::Method old_method, int32_t index,
                               hotspot::oops::Method new_method) noexcept;

    static void deoptimization_method(hotspot::oops::Method method) noexcept;

    static inline uint64_t addr;
};