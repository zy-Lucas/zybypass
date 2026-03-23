#include "codeBlob.hpp"

namespace hotspot::code
{
CodeBlob::CodeBlob(uint64_t addr) : runtime::JvmObject<CodeBlob>(addr) {}

void CodeBlob::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CodeBlob");

    name_offset = type->get_field("_name")->get_offset();
    size_offset = type->get_field("_size")->get_offset();
    header_size_offset = type->get_field("_header_size")->get_offset();
    frame_complete_offset_offset = type->get_field("_frame_complete_offset")->get_offset();
    content_begin_offset = type->get_field("_content_begin")->get_offset();
    code_begin_offset = type->get_field("_code_begin")->get_offset();
    code_end_offset = type->get_field("_code_end")->get_offset();
    data_end_offset = type->get_field("_data_end")->get_offset();
    data_offset_offset = type->get_field("_data_offset")->get_offset();
    frame_size_offset = type->get_field("_frame_size")->get_offset();
    oop_maps_offset = type->get_field("_oop_maps")->get_offset();
}

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
} // namespace hotspot::code