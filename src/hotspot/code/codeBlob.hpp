#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::code
{
class CodeBlob : public runtime::JvmObject
{
  public:
    CodeBlob(uint64_t addr) noexcept : runtime::JvmObject(addr) {}

    uint64_t header_begin() const noexcept { return address(); }
    uint64_t header_end() const noexcept { return address() + header_size(); }

    uint64_t content_begin() const noexcept { return read_field<uint64_t>(content_begin_offset_); }
    uint64_t content_end() const noexcept { return header_begin() + data_offset(); }

    uint64_t code_begin() const noexcept { return read_field<uint64_t>(code_begin_offset_); }
    uint64_t code_end() const noexcept { return read_field<uint64_t>(code_end_offset_); }

    uint64_t data_begin() const noexcept { return header_begin() + data_offset(); }
    uint64_t data_end() const noexcept { return read_field<uint64_t>(data_end_offset_); }

    uint32_t frame_complete_offset() const noexcept { return read_field<uint32_t>(frame_complete_offset_offset_); }
    uint32_t data_offset() const noexcept { return read_field<uint32_t>(data_offset_offset_); }

    uint32_t size() const noexcept { return read_field<uint32_t>(size_offset_); }
    uint32_t header_size() const noexcept { return read_field<uint32_t>(header_size_offset_); }
    uint32_t frame_size_word() const noexcept { return read_field<uint32_t>(frame_size_offset_); }
    std::string_view name_view() const noexcept { return read_string_field(name_offset_); }
    std::string name() const { return std::string{name_view()}; }

    size_t content_size() const noexcept { return content_end() - content_begin(); }
    size_t code_size() const noexcept { return code_end() - code_begin(); }
    size_t data_size() const noexcept { return data_end() - data_begin(); }

    bool blob_contains(uint64_t addr) const noexcept { return header_begin() <= addr && data_end() > addr; }
    bool content_contains(uint64_t addr) const noexcept { return content_begin() <= addr && content_end() > addr; }
    bool code_contains(uint64_t addr) const noexcept { return code_begin() <= addr && code_end() > addr; }
    bool data_contains(uint64_t addr) const noexcept { return data_begin() <= addr && data_end() > addr; }
    bool is_frame_complete_at(uint64_t addr) const noexcept;

    uint32_t frame_size() const noexcept { return sizeof(void *) * frame_size_word(); }

  private:
    friend std::ostream &operator<<(std::ostream &os, const CodeBlob &code_blob);

    DECLARE_STATIC_INIT

    static inline uint64_t name_offset_;
    static inline uint64_t size_offset_;
    static inline uint64_t header_size_offset_;
    static inline uint64_t frame_complete_offset_offset_;
    static inline uint64_t content_begin_offset_;
    static inline uint64_t code_begin_offset_;
    static inline uint64_t code_end_offset_;
    static inline uint64_t data_end_offset_;
    static inline uint64_t data_offset_offset_;
    static inline uint64_t frame_size_offset_;
    static inline uint64_t oop_maps_offset_;
};

class RuntimeBlob : public CodeBlob
{
  public:
    RuntimeBlob(uint64_t addr) : CodeBlob(addr) {}

  private:
    DECLARE_STATIC_INIT
};

class RuntimeStub : public RuntimeBlob
{
  public:
    RuntimeStub(uint64_t addr) : RuntimeBlob(addr) {}

    bool caller_must_gc_arguments() const noexcept { return read_field<bool>(caller_must_gc_arguments_offset_); }

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t caller_must_gc_arguments_offset_;
};
} // namespace hotspot::code