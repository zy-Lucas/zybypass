#include "javaTools.hpp"
#include "../hotspot/classfile/javaClasses.hpp"

std::string JavaTools::to_std_string(jstring string) noexcept
{
    return hotspot::classfile::java_lang_String::to_std_string((hotspot::oops::Instance) * (uint64_t *)string);
}