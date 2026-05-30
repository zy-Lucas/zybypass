#pragma once

#include "jni.h"
#include "string"

class JavaTools
{
  public:
    static std::string to_std_string(jstring string) noexcept;
};