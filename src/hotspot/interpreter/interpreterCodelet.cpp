#include "interpreterCodelet.hpp"

namespace hotspot::interpreter
{
uint64_t InterpreterCodelet::code_begin() const noexcept
{
    return address() + runtime::Jvm::align_up(instance_size_, runtime::Jvm::code_entry_alignment());
}

void InterpreterCodelet::initialize()
{
    utils::FieldResolver r{"InterpreterCodelet"};

    r.field_offset("_description", description_offset_);
    r.field_offset("_size", size_offset_);
    r.field_offset("_bytecode", bytecode_offset_);

    r.type_size(instance_size_);
}
}