#include "interpreterCodelet.hpp"

namespace hotspot::interpreter
{
uint64_t InterpreterCodelet::code_begin() const noexcept
{
    return address() + runtime::Jvm::align_up(instance_size_, runtime::Jvm::code_entry_alignment());
}

void InterpreterCodelet::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("InterpreterCodelet");

    description_offset_ = *type->field_offset("_description");
    size_offset_ = *type->field_offset("_size");
    bytecode_offset_ = *type->field_offset("_bytecode");

    instance_size_ = type->size();
}
}