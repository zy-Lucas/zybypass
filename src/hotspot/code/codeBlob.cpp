#include "codeBlob.hpp"

namespace hotspot::code
{
CodeBlob::CodeBlob(uint64_t addr) : runtime::JvmObject<CodeBlob>(addr) {}

std::ostream &operator<<(std::ostream &os, const CodeBlob &codeBlob)
{
    os << codeBlob.get_name() << std::endl
       << "{"
       << "content=[" << std::hex << codeBlob.content_begin() << ", " << codeBlob.content_end() << "), "
       << "code=[" << std::hex << codeBlob.code_begin() << ", " << codeBlob.code_end() << "), "
       << "data=[" << std::hex << codeBlob.data_begin() << ", " << codeBlob.data_end() << "), "
       << "frame size=" << std::dec << codeBlob.get_frame_size() << ", "
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
} // namespace hotspot::code