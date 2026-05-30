#pragma once

#include "../runtime/basicType.hpp"
#include "fieldType.hpp"
#include "instanceKlass.hpp"
#include "objHeap.hpp"
#include "oop.hpp"
#include "typeArrayKlass.hpp"
#include <variant>

namespace hotspot::oops
{
class NamedFieldIdentifier
{
  public:
    NamedFieldIdentifier(std::string_view name) noexcept : name_(name) {}

    std::string_view name_view() const noexcept { return name_; }
    std::string name() const { return std::string{name_}; }

    bool operator==(const NamedFieldIdentifier &o) const noexcept { return name_ == o.name_; }

    size_t hash_code() const noexcept { return std::hash<std::string_view>{}(name_); }

  private:
    std::string_view name_;
};

class IndexableFieldIdentifier
{
  public:
    IndexableFieldIdentifier(uint32_t index) noexcept : index_(index) {}

    uint32_t index() const noexcept { return index_; }
    std::string name() const { return std::to_string(index_); }

    bool operator==(const IndexableFieldIdentifier &o) const noexcept { return index_ == o.index_; }

    size_t hash_code() const noexcept { return index_; }

  private:
    uint32_t index_;
};

using FieldIdentifier = std::variant<NamedFieldIdentifier, IndexableFieldIdentifier>;

inline std::string name(const FieldIdentifier &id)
{
    return std::visit([](const auto &v) { return v.name(); }, id);
}

inline size_t hash_code(const FieldIdentifier &id)
{
    return std::visit([](const auto &v) { return v.hash_code(); }, id);
}

inline bool operator==(const FieldIdentifier &a, const FieldIdentifier &b)
{
    return std::visit(
        [](const auto &av, const auto &bv) {
            using T = std::decay_t<decltype(av)>;
            using U = std::decay_t<decltype(bv)>;
            if constexpr (std::is_same_v<T, U>)
                return av == bv;
            else
                return false;
        },
        a, b);
}

class Field
{
  public:
    Field(uint64_t offset, FieldIdentifier id, bool is_vm_field) noexcept;
    Field(InstanceKlass holder, uint32_t field_index) noexcept;

    uint64_t offset() const noexcept { return offset_; }
    const FieldIdentifier &id() const noexcept { return id_; }
    Symbol name() const noexcept { return name_; }

    bool is_vm_field() const noexcept { return vm_field_; }
    bool is_named_field() const noexcept { return std::holds_alternative<NamedFieldIdentifier>(id_); }

    uint32_t field_index() const noexcept { return field_index_; }
    InstanceKlass field_holder() const noexcept { return holder_; }

    uint64_t access_flags() const noexcept { return access_flags_.value(); };
    runtime::AccessFlags access_flags_obj() const noexcept { return access_flags_; };

    FieldType field_type() const noexcept { return field_type_; }

    Symbol signature() const noexcept { return signature_; }
    Symbol generic_signature() const noexcept { return generic_signature_; }

    bool has_initial_value() const noexcept { return holder_.field_initial_value_index(field_index_); }

    bool is_public() const noexcept { return access_flags_.is_public(); }
    bool is_private() const noexcept { return access_flags_.is_private(); }
    bool is_protected() const noexcept { return access_flags_.is_protected(); }
    bool is_package_private() const noexcept { return !is_public() && !is_private() && !is_protected(); }

    bool is_static() const noexcept { return access_flags_.is_static(); }
    bool is_final() const noexcept { return access_flags_.is_final(); }
    bool is_volatile() const noexcept { return access_flags_.is_volatile(); }
    bool is_transient() const noexcept { return access_flags_.is_transient(); }

    bool is_synthetic() const noexcept { return access_flags_.is_synthetic(); }
    bool is_enum_constant() const noexcept { return access_flags_.is_enum(); }

    bool operator==(const Field &other) const;

    explicit operator bool() const noexcept { return (bool)holder_; }

    size_t hash_code() const noexcept;

  private:
    Symbol name_{0};
    uint64_t offset_;
    FieldIdentifier id_;
    bool vm_field_ = false;

    InstanceKlass holder_{0};
    Symbol generic_signature_{0};
    Symbol signature_{0};
    FieldType field_type_{0};
    runtime::AccessFlags access_flags_{0};
    uint32_t field_index_ = 0;
};

#define DECLARE_PRIMITIVE_FIELD(ClassName, Type)                                                                       \
    class ClassName : public Field                                                                                     \
    {                                                                                                                  \
      public:                                                                                                          \
        ClassName(uint64_t offset, FieldIdentifier id, bool is_vm_field) noexcept                                      \
            : Field(offset, std::move(id), is_vm_field)                                                                \
        {                                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        ClassName(const Field &field) noexcept : Field(field) {}                                                       \
                                                                                                                       \
        ClassName(InstanceKlass holder, uint32_t field_array_index) noexcept : Field(holder, field_array_index) {}     \
                                                                                                                       \
        Type value(const Oop &obj) const noexcept                                                                      \
        {                                                                                                              \
            return runtime::Jvm::read<Type>(obj.handle().address() + offset());                                        \
        }                                                                                                              \
                                                                                                                       \
        void set_value(const Oop &obj, Type value) noexcept                                                            \
        {                                                                                                              \
            runtime::Jvm::write(obj.handle().address() + offset(), value);                                             \
        }                                                                                                              \
    };

DECLARE_PRIMITIVE_FIELD(ByteField, int8_t)
DECLARE_PRIMITIVE_FIELD(BooleanField, uint8_t)
DECLARE_PRIMITIVE_FIELD(CharField, uint16_t)
DECLARE_PRIMITIVE_FIELD(IntField, int32_t)
DECLARE_PRIMITIVE_FIELD(ShortField, int16_t)
DECLARE_PRIMITIVE_FIELD(LongField, int64_t)
DECLARE_PRIMITIVE_FIELD(FloatField, float)
DECLARE_PRIMITIVE_FIELD(DoubleField, double)

#undef DECLARE_PRIMITIVE_FIELD

class OopField : public Field
{
  public:
    OopField(uint64_t offset, FieldIdentifier id, bool is_vm_field) noexcept;
    OopField(InstanceKlass holder, uint32_t field_array_index) noexcept : Field(holder, field_array_index) {}
    OopField(const Field &field) noexcept : Field(field) {}

    Oop value(const Oop &obj) const { return ObjHeap::new_oop(value_as_oop_handle(obj)); }
    debugger::OopHandle value_as_oop_handle(const Oop &obj) const;

    void set_value(const Oop &obj, const Oop &value) noexcept;
};

template <typename Visitor> void TypeArray::iterate_fields(Visitor &&visitor)
{
    TypeArrayKlass k{klass().address()};
    uint32_t len = length();
    uint32_t type = k.element_type();
    uint64_t base = base_offset_in_bytes(type);

    for (uint32_t index = 0; index < len; ++index)
    {
        switch (type)
        {
#define CASE(Tag, FieldClass, ElemSize)                                                                                \
    case TypeArrayKlass::Tag:                                                                                          \
        visitor(FieldClass{base + index * ElemSize, index, false});                                                    \
        break
            CASE(T_BYTE, ByteField, sizeof(int8_t));
            CASE(T_BOOLEAN, BooleanField, sizeof(uint8_t));
            CASE(T_CHAR, CharField, sizeof(uint16_t));
            CASE(T_INT, IntField, sizeof(int32_t));
            CASE(T_SHORT, ShortField, sizeof(int16_t));
            CASE(T_LONG, LongField, sizeof(int64_t));
            CASE(T_FLOAT, FloatField, sizeof(float));
            CASE(T_DOUBLE, DoubleField, sizeof(double));
#undef CASE
        }
    }
}

template <typename Visitor> void ObjArray::iterate_fields(Visitor &&visitor)
{
    uint32_t len = length();
    uint64_t base = base_offset_in_bytes(runtime::BasicType::T_OBJECT);

    for (uint32_t index = 0; index < len; ++index)
    {
        uint64_t offset = base + (index * element_size);
        IndexableFieldIdentifier id(index);
        visitor(OopField(offset, id, false));
    }
}

template <typename Visitor> void InstanceKlass::visit_field(Visitor &&visitor, FieldType type, uint32_t index)
{
    switch (FieldType{field_signature(index)}.tag())
    {
    case 'B':
        visitor(ByteField{*this, index});
        break;
    case 'Z':
        visitor(BooleanField{*this, index});
        break;
    case 'C':
        visitor(CharField{*this, index});
        break;
    case 'I':
        visitor(IntField{*this, index});
        break;
    case 'S':
        visitor(ShortField{*this, index});
        break;
    case 'J':
        visitor(LongField{*this, index});
        break;
    case 'F':
        visitor(FloatField{*this, index});
        break;
    case 'D':
        visitor(DoubleField{*this, index});
        break;
    case 'L':
    case '[':
        visitor(OopField(*this, index));
        break;
    }
}
} // namespace hotspot::oops

namespace std
{
template <> struct hash<hotspot::oops::FieldIdentifier>
{
    size_t operator()(const hotspot::oops::FieldIdentifier &id) const { return hotspot::oops::hash_code(id); }
};
} // namespace std