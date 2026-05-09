#pragma once

#include "jvmObject.hpp"

namespace hotspot::runtime
{
class BasicType
{
  public:
    static int32_t char_to_type(uint8_t c) noexcept
    {
        if (c == 'B')
            return T_BYTE;
        if (c == 'C')
            return T_CHAR;
        if (c == 'D')
            return T_DOUBLE;
        if (c == 'F')
            return T_FLOAT;
        if (c == 'I')
            return T_INT;
        if (c == 'J')
            return T_LONG;
        if (c == 'S')
            return T_SHORT;
        if (c == 'Z')
            return T_BOOLEAN;
        if (c == 'V')
            return T_VOID;
        if (c == 'L')
            return T_OBJECT;
        if (c == '[')
            return T_ARRAY;
        return T_ILLEGAL;
    }

    static std::string_view to_string(int32_t type) noexcept
    {
        if (type == T_BOOLEAN)
            return "boolean";
        if (type == T_CHAR)
            return "char";
        if (type == T_FLOAT)
            return "float";
        if (type == T_DOUBLE)
            return "double";
        if (type == T_BYTE)
            return "byte";
        if (type == T_SHORT)
            return "short";
        if (type == T_INT)
            return "int";
        if (type == T_LONG)
            return "long";
        if (type == T_OBJECT)
            return "object";
        if (type == T_ARRAY)
            return "array";
        if (type == T_VOID)
            return "void";
        if (type == T_ADDRESS)
            return "address";
        if (type == T_NARROWOOP)
            return "narrow oop";
        if (type == T_METADATA)
            return "metadata";
        if (type == T_NARROWKLASS)
            return "narrow klass";
        if (type == T_CONFLICT)
            return "conflict";
        return "ILLEGAL TYPE";
    }

    static inline int32_t T_BOOLEAN;
    static inline int32_t T_CHAR;
    static inline int32_t T_FLOAT;
    static inline int32_t T_DOUBLE;
    static inline int32_t T_BYTE;
    static inline int32_t T_SHORT;
    static inline int32_t T_INT;
    static inline int32_t T_LONG;
    static inline int32_t T_OBJECT;
    static inline int32_t T_ARRAY;
    static inline int32_t T_VOID;
    static inline int32_t T_ADDRESS;
    static inline int32_t T_NARROWOOP;
    static inline int32_t T_METADATA;
    static inline int32_t T_NARROWKLASS;
    static inline int32_t T_CONFLICT;
    static inline int32_t T_ILLEGAL;

  private:
    DECLARE_STATIC_INIT
};
} // namespace hotspot::runtime