#pragma once

#include "../types/wrongTypeException.h"
#include "jvmObject.hpp"

namespace hotspot::runtime
{
class InstanceConstructor
{
  public:
    virtual ~InstanceConstructor() = default;

    virtual std::pair<std::string_view, JvmObjectBase> instantiate_wrapper_for(uint64_t addr) = 0;

  protected:
    types::WrongTypeException wrong_type_exception(uint64_t addr)
    {
        return types::WrongTypeException("No suitable match for type of address " + std::to_string(addr));
    }
};
} // namespace hotspot::runtime