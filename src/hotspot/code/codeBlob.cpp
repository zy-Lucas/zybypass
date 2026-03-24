#include "codeBlob.hpp"

namespace hotspot::code
{
CodeBlob::CodeBlob(uint64_t addr) : runtime::JvmObject<CodeBlob>(addr) {}

void CodeBlob::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("CodeBlob");

    name_offset = type->get_field_offset("_name").value_or(0);
    size_offset = type->get_field_offset("_size").value_or(0);
    header_size_offset = type->get_field_offset("_header_size").value_or(0);
    frame_complete_offset_offset = type->get_field_offset("_frame_complete_offset").value_or(0);
    content_begin_offset = type->get_field_offset("_content_begin").value_or(0);
    code_begin_offset = type->get_field_offset("_code_begin").value_or(0);
    code_end_offset = type->get_field_offset("_code_end").value_or(0);
    data_end_offset = type->get_field_offset("_data_end").value_or(0);
    data_offset_offset = type->get_field_offset("_data_offset").value_or(0);
    frame_size_offset = type->get_field_offset("_frame_size").value_or(0);
    oop_maps_offset = type->get_field_offset("_oop_maps").value_or(0);
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