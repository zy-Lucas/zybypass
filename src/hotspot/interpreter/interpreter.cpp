#include "interpreter.hpp"

namespace hotspot::interpreter
{
void Interpreter::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("AbstractInterpreter");

    code_offset_ = *type->field_offset("_code");
}
} // namespace hotspot::interpreter