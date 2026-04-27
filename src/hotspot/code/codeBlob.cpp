#include "codeBlob.hpp"

namespace hotspot::code
{
std::ostream &operator<<(std::ostream &os, const CodeBlob &code_blob)
{
    os << code_blob.get_name() << std::endl
       << "{"
       << "content=[" << std::hex << code_blob.content_begin() << ", " << code_blob.content_end() << "), "
       << "code=[" << std::hex << code_blob.code_begin() << ", " << code_blob.code_end() << "), "
       << "data=[" << std::hex << code_blob.data_begin() << ", " << code_blob.data_end() << "), "
       << "frame size=" << std::dec << code_blob.get_frame_size() << ", "
       << "}";
    return os;
}

void CodeBlob::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CodeBlob");

    name_offset = *type->get_field_offset("_name");
    size_offset = *type->get_field_offset("_size");
    header_size_offset = *type->get_field_offset("_header_size");
    frame_complete_offset_offset = *type->get_field_offset("_frame_complete_offset");
    content_begin_offset = *type->get_field_offset("_content_begin");
    code_begin_offset = *type->get_field_offset("_code_begin");
    code_end_offset = *type->get_field_offset("_code_end");
    data_end_offset = *type->get_field_offset("_data_end");
    data_offset_offset = *type->get_field_offset("_data_offset");
    frame_size_offset = *type->get_field_offset("_frame_size");
    oop_maps_offset = *type->get_field_offset("_oop_maps");
}

void RuntimeBlob::initialize()
{
    // types::Type *type = runtime::Jvm::lookup_type("RuntimeBlob");
}

void RuntimeStub::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("RuntimeStub");

    caller_must_gc_arguments_offset = *type->get_field_offset("_caller_must_gc_arguments");
}
} // namespace hotspot::code