#include "interpreter.hpp"

namespace hotspot::interpreter
{
void Interpreter::initialize()
{
    utils::FieldResolver r{"AbstractInterpreter"};

    r.field_offset("_code", code_offset_);
}
} // namespace hotspot::interpreter