#include "oop.hpp"
#include "../memory/universe.hpp"
#include "arrayKlass.hpp"

namespace hotspot::oops
{
Klass Oop::klass() const noexcept
{
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        return handle().compressed_klass_address_at(compressed_klass_offset_);
    return runtime::Jvm::read<uint64_t>(handle().address() + klass_offset_);
}

uint64_t Oop::align_object_size(uint64_t size) noexcept
{
    return runtime::Jvm::align_up(size, runtime::Jvm::object_alignment_in_bytes());
}

Klass Oop::klass_for_oop_handle(debugger::OopHandle handle) noexcept
{
    if (!handle)
        return 0;
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        return handle.compressed_klass_address_at(compressed_klass_offset_);
    return runtime::Jvm::read<uint64_t>(handle.address() + klass_offset_);
}

void Oop::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("oopDesc");

    mark_offset_ = *type->field_offset("_mark");
    klass_offset_ = *type->field_offset("_metadata._klass");
    compressed_klass_offset_ = *type->field_offset("_metadata._compressed_klass");

    header_size_ = type->size();
}

uint64_t Array::length() const noexcept
{
    return runtime::Jvm::read<uint32_t>(handle().address() + length_offset_in_bytes());
}

uint64_t Array::array_object_size() const noexcept
{
    ArrayKlass array{klass().address()};
    return align_object_size((length() << array.log2_element_size()) + array.array_header_in_bytes());
}

uint64_t Array::base_offset_in_bytes(int32_t type) noexcept
{
    return header_size(type) * runtime::Jvm::heap_word_size();
}

uint64_t Array::header_size_in_bytes() noexcept
{
    static uint64_t header_size =
        runtime::Jvm::is_compressed_klass_pointers_enabled()
            ? type_size_
            : runtime::Jvm::align_up(type_size_ + sizeof(int32_t), runtime::Jvm::heap_word_size());
    return header_size;
}

uint64_t Array::header_size(int32_t type) noexcept
{
    static uint64_t base_words = header_size_in_bytes() / runtime::Jvm::heap_word_size();
    static uint64_t aligned_words = align_object_size(header_size_in_bytes()) / runtime::Jvm::heap_word_size();
    return memory::Universe::element_type_should_be_aligned(type) ? aligned_words : base_words;
}

uint64_t Array::length_offset_in_bytes() noexcept
{
    static uint64_t length_offset_in_bytes =
        runtime::Jvm::is_compressed_klass_pointers_enabled() ? type_size_ - sizeof(int32_t) : type_size_;
    return length_offset_in_bytes;
}

void Array::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("arrayOopDesc");

    type_size_ = type->size();
}
} // namespace hotspot::oops