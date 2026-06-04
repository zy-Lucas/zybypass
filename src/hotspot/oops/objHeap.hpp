#pragma once

#include "debugger/oopHandle.hpp"
#include "oop.hpp"

namespace hotspot::oops
{
class ObjHeap
{
  public:
    static oops::Oop new_oop(debugger::OopHandle handle) noexcept;
};
} // namespace hotspot::oops