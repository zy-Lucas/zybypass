#pragma once

#include "oops/instanceKlass.hpp"
#include <jni.h>

class JavaTools
{
  public:
    static std::string to_std_string(jstring string) noexcept;

    static std::pair<hotspot::oops::Method, int32_t> lookup_method(std::string_view klass_name,
                                                                   std::string_view method_name,
                                                                   std::string_view method_sign);

    static void set_method_to_native(jstring klass_name, jstring method_name, jstring method_sign, uint64_t native_addr,
                                     JNIEnv *env = nullptr) noexcept;
    static void set_method_to_native(const std::string &klass_name, const std::string &method_name,
                                     const std::string &method_sign, uint64_t native_addr,
                                     JNIEnv *env = nullptr) noexcept;

    static void replace_method(hotspot::oops::InstanceKlass ik, hotspot::oops::Method old_method, int32_t index,
                               hotspot::oops::Method new_method) noexcept;

    static void deoptimization_method(hotspot::oops::Method method) noexcept;

    static jclass find_class(JNIEnv *env, const std::string &slash_name);

    static void adjust_method_handle_entry(JNIEnv *env, jclass target_class, const std::string &method_name,
                                           const std::string &method_sign, hotspot::oops::Method new_method);

  private:
    static void init(JNIEnv *env);

    static inline struct
    {
        jclass mt_class;
        jmethodID from_desc;
        jclass mn_class;
        jmethodID ctor;
        jclass factory_class;
        jfieldID instance_field;
        jmethodID resolve;
        jclass resolved_method_class;
        jfieldID method_field;
    } g_cache{};
};