#include "oop.hpp"
#include "arrayKlass.hpp"
#include "memory/universe.hpp"
#include "objHeap.hpp"
#include "runtime/basicType.hpp"
#include "runtime/jvm.hpp"

namespace hotspot::oops
{
Klass Oop::klass() const noexcept
{
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        return handle().compressed_klass_address_at(compressed_klass_offset_);
    return runtime::Jvm::read<uint64_t>(handle().address() + klass_offset_);
}

void Oop::set_klass(uint64_t klass) noexcept
{
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        handle().write_compressed_klass_address_at(compressed_klass_offset_, klass);
    runtime::Jvm::write(handle().address() + klass_offset_, klass);
}

uint64_t Oop::align_object_size(uint64_t size) noexcept
{
    return runtime::Jvm::align_up(size, runtime::Jvm::min_obj_alignment_in_bytes());
}

Klass Oop::klass_for_oop_handle(debugger::OopHandle handle) noexcept
{
    if (!handle)
        return 0;
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        return handle.compressed_klass_address_at(compressed_klass_offset_);
    return runtime::Jvm::read<uint64_t>(handle.address() + klass_offset_);
}

void Oop::set_klass_gap(uint64_t mem, int32_t v) noexcept
{
    if (runtime::Jvm::is_compressed_klass_pointers_enabled())
        *(int32_t *)(mem + klass_gap_offset_in_bytes()) = v;
}

void Oop::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("oopDesc");

    mark_offset_ = *type->field_offset("_mark");
    klass_offset_ = *type->field_offset("_metadata._klass");
    compressed_klass_offset_ = *type->field_offset("_metadata._compressed_klass");

    header_size_ = type->size();
}

uint32_t Array::length() const noexcept
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

uint64_t Instance::header_size() noexcept
{
    static uint64_t header_size =
        runtime::Jvm::is_compressed_klass_pointers_enabled() ? type_size_ - sizeof(int32_t) : type_size_;
    return type_size_;
}

void Instance::initialize()
{
    types::Type *type = runtime::Jvm::lookup_type("instanceOopDesc");

    type_size_ = type->size();
}

int8_t *TypeArray::byte_base() const noexcept
{
    return (int8_t *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_BYTE));
}

uint8_t *TypeArray::bool_base() const noexcept
{
    return (uint8_t *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_BOOLEAN));
}

uint16_t *TypeArray::char_base() const noexcept
{
    return (uint16_t *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_CHAR));
}

int32_t *TypeArray::int_base() const noexcept
{
    return (int32_t *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_INT));
}

int16_t *TypeArray::short_base() const noexcept
{
    return (int16_t *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_SHORT));
}

int64_t *TypeArray::long_base() const noexcept
{
    return (int64_t *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_LONG));
}

float *TypeArray::float_base() const noexcept
{
    return (float *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_FLOAT));
}

double *TypeArray::double_base() const noexcept
{
    return (double *)(handle().address() + base_offset_in_bytes(runtime::BasicType::T_DOUBLE));
}

void TypeArray::initialize()
{
    // types::Type *type = runtime::Jvm::lookup_type("typeArrayOop");
}

debugger::OopHandle ObjArray::oop_handle_at(uint32_t index) const
{
    static uint64_t offset = base_offset_in_bytes(runtime::BasicType::T_OBJECT) + index * element_size;
    if (runtime::Jvm::is_compressed_oops_enabled())
        return handle().compressed_oop_handle_at(offset);
    return handle().oop_handle_at(offset);
}

Oop ObjArray::obj_at(uint32_t index) const noexcept { return ObjHeap::new_oop(oop_handle_at(index)); }

void ObjArray::initialize()
{
    // types::Type *type = runtime::Jvm::lookup_type("objArrayOopDesc");

    element_size = runtime::Jvm::heap_oop_size();
}
} // namespace hotspot::oops