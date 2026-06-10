#pragma once

#include "debugger/oopHandle.hpp"
#include "instanceKlass.hpp"
#include "mark.hpp"
#include "oops/klass.hpp"
#include "runtime/jvm.hpp"

namespace hotspot::oops
{
class Oop
{
  public:
    enum class Kind : uint8_t
    {
        instance,
        type_array,
        obj_array,
        non_type
    };

    Oop(Kind k, debugger::OopHandle h) noexcept : kind_(k), handle_(h) {}

    debugger::OopHandle handle() const noexcept { return handle_; }
    Kind kind() const noexcept { return kind_; }

    bool is_instance() const noexcept { return kind_ == Kind::instance; }
    bool is_array() const noexcept { return kind_ == Kind::type_array || kind_ == Kind::obj_array; }
    bool is_type_array() const noexcept { return kind_ == Kind::type_array; }
    bool is_obj_array() const noexcept { return kind_ == Kind::obj_array; }

    Mark mark() const noexcept { return handle().address(); }
    void set_mark(Mark mark) noexcept { runtime::Jvm::write(handle().address() + mark_offset_, mark.address()); };

    Klass klass() const noexcept;
    void set_klass(Klass klass) noexcept;

    bool operator==(const Oop &o) const noexcept { return handle_ == o.handle_; }

    explicit operator bool() const noexcept { return (bool)handle_; }

    size_t hash_code() const noexcept { return handle_.hash_code(); }

    // Instance as_instance() const noexcept;
    // TypeArray as_type_array() const noexcept;
    // ObjArray as_obj_array() const noexcept;

    template <typename Visitor> void iterate_fields(Visitor &&visitor);

    static uint64_t header_size() noexcept { return header_size_; }

    static uint64_t align_object_size(uint64_t size) noexcept;

    static Klass klass_for_oop_handle(debugger::OopHandle handle) noexcept;

    static void set_klass_gap(uint64_t mem, int32_t v) noexcept;

    static int klass_gap_offset_in_bytes() noexcept { return compressed_klass_offset_ + sizeof(int32_t); }

  private:
    Kind kind_;
    debugger::OopHandle handle_;

    DECLARE_STATIC_INIT

    static inline uint64_t mark_offset_;
    static inline uint64_t klass_offset_;
    static inline uint64_t compressed_klass_offset_;

    static inline uint64_t header_size_;
};

class Array : public Oop
{
  public:
    Array(Kind k, debugger::OopHandle h) noexcept : Oop(k, h) {}
    Array(const Oop &o) noexcept : Oop(o) {}

    uint32_t length() const noexcept;
    uint64_t array_object_size() const noexcept;

    static uint64_t base_offset_in_bytes(int32_t type) noexcept;

  private:
    static uint64_t header_size_in_bytes() noexcept;
    static uint64_t header_size(int32_t type) noexcept;

    static uint64_t length_offset_in_bytes() noexcept;

    DECLARE_STATIC_INIT

    static inline uint64_t type_size_;
};

class Instance : public Oop
{
  public:
    Instance(debugger::OopHandle h) noexcept : Oop(Kind::instance, h) {}
    Instance(const Oop &o) noexcept : Oop(o) {}

    static uint64_t header_size() noexcept;

    template <typename Visitor> void iterate_fields(Visitor &&visitor);

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t type_size_;
};

class TypeArray : public Array
{
  public:
    TypeArray(debugger::OopHandle h) noexcept : Array(Kind::type_array, h) {}
    TypeArray(const Oop &o) noexcept : Array(o) {}

    int8_t *byte_base() const noexcept;
    int8_t *byte_at_addr(uint32_t index) const noexcept { return byte_base() + index; }
    int8_t byte_at(uint32_t index) const noexcept { return *byte_at_addr(index); }

    uint8_t *bool_base() const noexcept;
    uint8_t *bool_at_addr(uint32_t index) const noexcept { return &bool_base()[index]; }
    bool bool_at(uint32_t index) const noexcept { return *bool_at_addr(index); }

    uint16_t *char_base() const noexcept;
    uint16_t *char_at_addr(uint32_t index) const noexcept { return &char_base()[index]; }
    uint16_t char_at(uint32_t index) const noexcept { return *char_at_addr(index); }

    int32_t *int_base() const noexcept;
    int32_t *int_at_addr(uint32_t index) const noexcept { return &int_base()[index]; }
    int32_t int_at(uint32_t index) const noexcept { return *int_at_addr(index); }

    int16_t *short_base() const noexcept;
    int16_t *short_at_addr(uint32_t index) const noexcept { return &short_base()[index]; }
    int16_t short_at(uint32_t index) const noexcept { return *short_at_addr(index); }

    int64_t *long_base() const noexcept;
    int64_t *long_at_addr(uint32_t index) const noexcept { return &long_base()[index]; }
    int64_t long_at(uint32_t index) const noexcept { return *long_at_addr(index); }

    float *float_base() const noexcept;
    float *float_at_addr(uint32_t index) const noexcept { return &float_base()[index]; }
    float float_at(uint32_t index) const noexcept { return *float_at_addr(index); }

    double *double_base() const noexcept;
    double *double_at_addr(uint32_t index) const noexcept { return &double_base()[index]; }
    double double_at(uint32_t index) const noexcept { return *double_at_addr(index); }

    uint64_t object_size() const { return array_object_size(); }

    static uint64_t object_size_of(const Oop &o) { return TypeArray{o}.array_object_size(); }

    template <typename Visitor> void iterate_fields(Visitor &&visitor);

  private:
    DECLARE_STATIC_INIT
};

class ObjArray : public Array
{
  public:
    ObjArray(debugger::OopHandle h) noexcept : Array(Kind::obj_array, h) {}
    ObjArray(const Oop &o) noexcept : Array(o) {}

    debugger::OopHandle oop_handle_at(uint32_t index) const;
    Oop obj_at(uint32_t index) const noexcept;

    uint64_t object_size() const { return array_object_size(); }

    static uint64_t object_size_of(const Oop &o) { return ObjArray{o}.array_object_size(); }

    template <typename Visitor> void iterate_fields(Visitor &&visitor);

  private:
    DECLARE_STATIC_INIT

    static inline uint64_t element_size;
};

template <typename Visitor> void Oop::iterate_fields(Visitor &&visitor)
{
    switch (kind_)
    {
    case Kind::instance:
        Instance{*this}.iterate_fields(std::forward<Visitor>(visitor));
        break;
    case Kind::type_array:
        TypeArray{*this}.iterate_fields(std::forward<Visitor>(visitor));
        break;
    case Kind::obj_array:
        ObjArray{*this}.iterate_fields(std::forward<Visitor>(visitor));
        break;
    case Kind::non_type:
        break;
    }
}

template <typename Visitor> void Instance::iterate_fields(Visitor &&visitor)
{
    visitor.set_oop(*this);
    InstanceKlass{klass().address()}.iterate_non_static_fields(visitor, *this);
}

template <typename Visitor> void InstanceKlass::iterate_static_fields(Visitor &&visitor)
{
    visitor.set_oop(java_mirror());
    uint16_t length = all_fields_count();
    for (uint32_t index = 0; index < length; ++index)
        if (runtime::AccessFlags access{field_access_flags(index)}; access.is_static())
            visit_field(visitor, FieldType(field_signature(index)), index);
}

template <typename Visitor> void InstanceKlass::iterate_non_static_fields(Visitor &&visitor, const Oop &obj)
{
    if (InstanceKlass ik{super().address()}; ik)
        ik.iterate_non_static_fields(visitor, obj);
    uint16_t length = all_fields_count();
    for (uint32_t index = 0; index < length; ++index)
        if (runtime::AccessFlags access{field_access_flags(index)}; !access.is_static())
            visit_field(visitor, FieldType(field_signature(index)), index);
}
} // namespace hotspot::oops