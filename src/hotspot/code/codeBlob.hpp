#pragma once

#include "../runtime/jvmObject.hpp"

namespace hotspot::code
{
class CodeBlob : public runtime::JvmObject<CodeBlob>
{
  public:
    CodeBlob(uint64_t addr);

    uint64_t header_begin() const noexcept { return address(); }
    uint64_t header_end() const noexcept { return address() + get_header_size(); }

    uint64_t content_begin() const noexcept { return read_field<uint64_t>(content_begin_offset); }
    uint64_t content_end() const noexcept { return header_begin() + get_data_offset(); }

    uint64_t code_begin() const noexcept { return read_field<uint64_t>(code_begin_offset); }
    uint64_t code_end() const noexcept { return read_field<uint64_t>(code_end_offset); }

    uint64_t data_begin() const noexcept { return header_begin() + get_data_offset(); }
    uint64_t data_end() const noexcept { return read_field<uint64_t>(data_end_offset); }

    uint32_t get_frame_complete_offset() const noexcept { return read_field<uint32_t>(frame_complete_offset_offset); }
    uint32_t get_data_offset() const noexcept { return read_field<uint32_t>(data_offset_offset); }

    uint32_t get_size() const noexcept { return read_field<uint32_t>(size_offset); }
    uint32_t get_header_size() const noexcept { return read_field<uint32_t>(header_size_offset); }
    uint32_t get_frame_size_word() const noexcept { return read_field<uint32_t>(frame_size_offset); }
    std::string_view get_name() const { return read_string_field(name_offset); }

    bool is_bufferBlob() const noexcept { return false; }
    bool is_compiled() const noexcept { return false; }
    bool is_nMethod() const noexcept { return false; }
    bool is_runtimeStub() const noexcept { return false; }
    bool is_deoptimizationStub() const noexcept { return false; }
    bool is_uncommonTrapStub() const noexcept { return false; }
    bool is_exceptionStub() const noexcept { return false; }
    bool is_safepointStub() const noexcept { return false; }
    bool is_adapterBlob() const noexcept { return false; }
    bool is_javaMethod() const noexcept { return false; }
    bool is_nativeMethod() const noexcept { return false; }
    bool is_osrMethod() const noexcept { return false; }

    size_t getContentSize() const noexcept { return content_end() - content_begin(); }
    size_t getCodeSize() const noexcept { return code_end() - code_begin(); }
    size_t getDataSize() const noexcept { return data_end() - data_begin(); }

    bool blob_contains(uint64_t addr) const noexcept { return header_begin() <= addr && data_end() > addr; }
    bool content_contains(uint64_t addr) const noexcept { return content_begin() <= addr && content_end() > addr; }
    bool code_contains(uint64_t addr) const noexcept { return code_begin() <= addr && code_end() > addr; }
    bool data_contains(uint64_t addr) const noexcept { return data_begin() <= addr && data_end() > addr; }
    bool is_frame_complete_at(uint64_t addr) const noexcept
    {
        return code_contains(addr) && addr - code_begin() >= get_frame_complete_offset();
    }

    bool is_zombie() const noexcept { return false; }
    bool is_locked_by_vm() const noexcept { return false; }

    uint32_t get_frame_size() const noexcept { return sizeof(void *) * get_frame_size_word(); }

  private:
    friend class JvmObject<CodeBlob>;
    friend std::ostream &operator<<(std::ostream &os, const CodeBlob &codeBlob);

    static inline std::once_flag init_flag_;

    static inline uint64_t name_offset;
    static inline uint64_t size_offset;
    static inline uint64_t header_size_offset;
    static inline uint64_t frame_complete_offset_offset;
    static inline uint64_t content_begin_offset;
    static inline uint64_t code_begin_offset;
    static inline uint64_t code_end_offset;
    static inline uint64_t data_end_offset;
    static inline uint64_t data_offset_offset;
    static inline uint64_t frame_size_offset;
    static inline uint64_t oop_maps_offset;

    static void initialize();
};
} // namespace hotspot::code