#pragma once

#include "../debugger/oopHandle.hpp"
#include "instanceKlass.hpp"
#include "klass.hpp"
#include "mark.hpp"

namespace hotspot::oops
{
class Oop
{
  public:
    enum class Kind : uint8_t
    {
        instance,
        type_array,
        obj_array
    };

    debugger::OopHandle get_handle() const noexcept { return handle; }
    Kind get_kind() const noexcept { return kind; }

    bool is_instance() const noexcept { return kind == Kind::instance; }
    bool is_array() const noexcept { return kind == Kind::type_array || kind == Kind::obj_array; }
    bool is_type_array() const noexcept { return kind == Kind::type_array; }
    bool is_obj_array() const noexcept { return kind == Kind::obj_array; }

    Mark get_mark() const noexcept { return get_handle().address(); }
    Klass get_klass() const noexcept;

    bool operator==(const Oop &o) const noexcept { return handle == o.handle; }

    size_t hash_code() const noexcept { return handle.hash_code(); }

    // Instance as_instance() const noexcept;
    // TypeArray as_type_array() const noexcept;
    // ObjArray as_obj_array() const noexcept;

    // template <typename Visitor> void iterate_fields(Visitor &&visitor, bool do_vm_fields) const;

    static Klass get_klass_for_oop_handle(debugger::OopHandle handle);

  protected:
    Oop(Kind k, debugger::OopHandle h) noexcept : kind(k), handle(h) {}

  private:
    Kind kind;
    debugger::OopHandle handle;

    DECLARE_STATIC_INIT

    static inline uint64_t mark_offset;
    static inline uint64_t klass_offset;
    static inline uint64_t compressed_klass_offset;

    static inline uint64_t header_size;
};

class Array : public Oop
{
  public:
    uint64_t length() const;

    static uint64_t base_offset_in_bytes(int32_t type);
    static uint64_t length_offset_in_bytes();

    uint64_t array_object_size() const;

  protected:
    Array(Kind k, debugger::OopHandle h) noexcept : Oop(k, h) {}

  private:
    static uint64_t header_size_in_bytes();
    static long header_size(int32_t type);

    DECLARE_STATIC_INIT

    static inline uint64_t type_size;
};

// class Instance final : public Oop
// {
//     static inline uint64_t type_size{0};

//   public:
//     Instance(debugger::OopHandle h) noexcept : Oop(Kind::instance, h) {}
//     explicit Instance(const Oop &o) noexcept : Oop(o) {}

//     static uint64_t get_header_size() noexcept;

//     void print_value_on(std::ostream &os) const;

//     template <typename Visitor> void iterate_fields(Visitor &&visitor, bool do_vm_fields) const
//     {
//         if (do_vm_fields)
//         {
//             visitor(LongField(NamedFieldIdentifier("_mark"), mark_off, true));
//             if (runtime::Jvm::is_compressed_klass_pointers_enabled())
//             {
//                 visitor(NarrowOopField(NamedFieldIdentifier("_compressed_klass"), compressed_klass_off, true));
//             }
//             else
//             {
//                 visitor(OopField(NamedFieldIdentifier("_klass"), klass_off, true));
//             }
//         }
//         InstanceKlass{klass()}.iterate_non_static_fields(visitor, *this);
//     }

//     static void initialize();
// };

// class TypeArray final : public Array
// {
//   public:
//     TypeArray(debugger::OopHandle h) noexcept : Array(Kind::type_array, h) {}
//     explicit TypeArray(const Oop &o) noexcept : Array(o) {}

//     int8_t byte_at(uint64_t index) const;
//     bool boolean_at(uint64_t index) const;
//     char16_t char_at(uint64_t index) const;
//     int32_t int_at(uint64_t index) const;
//     int16_t short_at(uint64_t index) const;
//     int64_t long_at(uint64_t index) const;
//     float float_at(uint64_t index) const;
//     double double_at(uint64_t index) const;

//     uint64_t object_size() const { return array_object_size(); }
//     static uint64_t object_size_of(const Oop &o) { return TypeArray{o}.array_object_size(); }

//     void print_value_on(std::ostream &os) const;

//     template <typename Visitor> void iterate_fields(Visitor &&visitor, bool do_vm_fields) const
//     {
//         if (do_vm_fields)
//         {
//             visitor(LongField(NamedFieldIdentifier("_mark"), mark_off, true));
//             if (runtime::Jvm::is_compressed_klass_pointers_enabled())
//             {
//                 visitor(NarrowOopField(NamedFieldIdentifier("_compressed_klass"), compressed_klass_off, true));
//             }
//             else
//             {
//                 visitor(OopField(NamedFieldIdentifier("_klass"), klass_off, true));
//             }
//         }

//         auto k = static_cast<TypeArrayKlass>(klass());
//         uint64_t len = length();
//         int type = k.get_element_type();

//         for (uint64_t index = 0; index < len; ++index)
//         {
//             IndexableFieldIdentifier id(static_cast<uint32_t>(index));
//             switch (type)
//             {
//             case TypeArrayKlass::T_BOOLEAN: {
//                 uint64_t off =
//                     base_offset_in_bytes(runtime::BasicType::T_BOOLEAN) + index * runtime::Jvm::get_boolean_size();
//                 visitor(BooleanField(id, off, false));
//                 break;
//             }
//             case TypeArrayKlass::T_CHAR: {
//                 uint64_t off = base_offset_in_bytes(runtime::BasicType::T_CHAR) + index *
//                 runtime::Jvm::get_char_size(); visitor(CharField(id, off, false)); break;
//             }
//             case TypeArrayKlass::T_FLOAT: {
//                 uint64_t off =
//                     base_offset_in_bytes(runtime::BasicType::T_FLOAT) + index * runtime::Jvm::get_float_size();
//                 visitor(FloatField(id, off, false));
//                 break;
//             }
//             case TypeArrayKlass::T_DOUBLE: {
//                 uint64_t off =
//                     base_offset_in_bytes(runtime::BasicType::T_DOUBLE) + index * runtime::Jvm::get_double_size();
//                 visitor(DoubleField(id, off, false));
//                 break;
//             }
//             case TypeArrayKlass::T_BYTE: {
//                 uint64_t off = base_offset_in_bytes(runtime::BasicType::T_BYTE) + index *
//                 runtime::Jvm::get_byte_size(); visitor(ByteField(id, off, false)); break;
//             }
//             case TypeArrayKlass::T_SHORT: {
//                 uint64_t off =
//                     base_offset_in_bytes(runtime::BasicType::T_SHORT) + index * runtime::Jvm::get_short_size();
//                 visitor(ShortField(id, off, false));
//                 break;
//             }
//             case TypeArrayKlass::T_INT: {
//                 uint64_t off = base_offset_in_bytes(runtime::BasicType::T_INT) + index *
//                 runtime::Jvm::get_int_size(); visitor(IntField(id, off, false)); break;
//             }
//             case TypeArrayKlass::T_LONG: {
//                 uint64_t off = base_offset_in_bytes(runtime::BasicType::T_LONG) + index *
//                 runtime::Jvm::get_long_size(); visitor(LongField(id, off, false)); break;
//             }
//             }
//         }
//     }

//     static void initialize();
// };

// // ============================================================================
// // ObjArray
// // ============================================================================
// class ObjArray final : public Array
// {
//     static inline uint64_t element_size{0};

//   public:
//     ObjArray(OopHandle h, ObjectHeap *p) noexcept : Array(Kind::obj_array, h, p) {}
//     explicit ObjArray(const Oop &o) noexcept : Array(o) {}

//     OopHandle oop_handle_at(uint64_t index) const;
//     Oop obj_at(uint64_t index) const;

//     uint64_t object_size() const { return array_object_size(); }
//     static uint64_t object_size_of(const Oop &o) { return ObjArray{o}.array_object_size(); }

//     void print_value_on(std::ostream &os) const;

//     template <typename Visitor> void iterate_fields(Visitor &&visitor, bool do_vm_fields) const
//     {
//         if (do_vm_fields)
//         {
//             visitor(LongField(NamedFieldIdentifier("_mark"), mark_off, true));
//             if (runtime::Jvm::is_compressed_klass_pointers_enabled())
//             {
//                 visitor(NarrowOopField(NamedFieldIdentifier("_compressed_klass"), compressed_klass_off, true));
//             }
//             else
//             {
//                 visitor(OopField(NamedFieldIdentifier("_klass"), klass_off, true));
//             }
//         }

//         uint64_t len = length();
//         uint64_t base = base_offset_in_bytes(runtime::BasicType::T_OBJECT);

//         for (uint64_t index = 0; index < len; ++index)
//         {
//             uint64_t offset = base + (index * element_size);
//             IndexableFieldIdentifier id(static_cast<uint32_t>(index));
//             if (runtime::Jvm::is_compressed_oops_enabled())
//             {
//                 visitor(NarrowOopField(id, offset, false));
//             }
//             else
//             {
//                 visitor(OopField(id, offset, false));
//             }
//         }
//     }

//     static void initialize();
// };

// template <typename Visitor> void Oop::iterate_fields(Visitor &&visitor, bool do_vm_fields) const
// {
//     switch (kind_)
//     {
//     case Kind::instance:
//         Instance{*this}.iterate_fields(std::forward<Visitor>(visitor), do_vm_fields);
//         break;
//     case Kind::type_array:
//         TypeArray{*this}.iterate_fields(std::forward<Visitor>(visitor), do_vm_fields);
//         break;
//     case Kind::obj_array:
//         ObjArray{*this}.iterate_fields(std::forward<Visitor>(visitor), do_vm_fields);
//         break;
//     }
// }
} // namespace hotspot::oops