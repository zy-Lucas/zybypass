#include "codeBlob.hpp"

namespace hotspot::code
{
bool CodeBlob::is_frame_complete_at(uint64_t addr) const noexcept
{
    return code_contains(addr) && addr - code_begin() >= frame_complete_offset();
}

std::ostream &operator<<(std::ostream &os, const CodeBlob &code_blob)
{
    os << code_blob.name() << std::endl
       << "{"
       << "content=[" << std::hex << code_blob.content_begin() << ", " << code_blob.content_end() << "), "
       << "code=[" << std::hex << code_blob.code_begin() << ", " << code_blob.code_end() << "), "
       << "data=[" << std::hex << code_blob.data_begin() << ", " << code_blob.data_end() << "), "
       << "frame size=" << std::dec << code_blob.frame_size() << ", "
       << "}";
    return os;
}

void CodeBlob::initialize()
{
    utils::FieldResolver r{"CodeBlob"};

    r.field_offset("_name", name_offset_);
    r.field_offset("_size", size_offset_);
    r.field_offset("_header_size", header_size_offset_);
    r.field_offset("_frame_complete_offset", frame_complete_offset_offset_);
    r.field_offset("_content_begin", content_begin_offset_);
    r.field_offset("_code_begin", code_begin_offset_);
    r.field_offset("_code_end", code_end_offset_);
    r.field_offset("_data_end", data_end_offset_);
    r.field_offset("_data_offset", data_offset_offset_);
    r.field_offset("_frame_size", frame_size_offset_);
    r.field_offset("_oop_maps", oop_maps_offset_);
}

void RuntimeBlob::initialize()
{
    // utils::FieldResolver r("RuntimeBlob");
}

void RuntimeStub::initialize()
{
    utils::FieldResolver r{"RuntimeStub"};

    r.field_offset("_caller_must_gc_arguments", caller_must_gc_arguments_offset_);
}
} // namespace hotspot::code