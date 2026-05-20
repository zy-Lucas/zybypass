#pragma once

#include "instanceKlass.hpp"
#include "oop.hpp"
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

class FieldType
{
  public:
    FieldType(Symbol signature) noexcept : signature_(signature), first_(signature.byte_at(0)) {}

    bool is_oop() const noexcept { return is_object() || is_array(); }
    bool is_byte() const noexcept { return first_ == 'B'; }
    bool is_char() const noexcept { return first_ == 'C'; }
    bool is_double() const noexcept { return first_ == 'D'; }
    bool is_float() const noexcept { return first_ == 'F'; }
    bool is_int() const noexcept { return first_ == 'I'; }
    bool is_long() const noexcept { return first_ == 'J'; }
    bool is_short() const noexcept { return first_ == 'S'; }
    bool is_boolean() const noexcept { return first_ == 'Z'; }
    bool is_object() const noexcept { return first_ == 'L'; }
    bool is_array() const noexcept { return first_ == '['; }

    Symbol signature() const noexcept { return signature_; }

    struct ArrayInfo
    {
        int32_t dimension_;
        int32_t element_basic_type_;
    };
    ArrayInfo get_array_info() const noexcept;

  private:
    static void skip_optional_size(Symbol sig, uint32_t &index) noexcept;

    Symbol signature_;
    uint8_t first_;
};

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
    InstanceKlass get_field_holder() const noexcept { return holder_; }

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

DECLARE_PRIMITIVE_FIELD(CharField, int16_t)
DECLARE_PRIMITIVE_FIELD(BooleanField, bool)
DECLARE_PRIMITIVE_FIELD(ByteField, int8_t)
DECLARE_PRIMITIVE_FIELD(DoubleField, double)
DECLARE_PRIMITIVE_FIELD(FloatField, float)
DECLARE_PRIMITIVE_FIELD(IntField, int32_t)
DECLARE_PRIMITIVE_FIELD(LongField, int64_t)

#undef DECLARE_PRIMITIVE_FIELD

// class OopField : public Field
// {
//   public:
//     OopField(uint64_t offset, FieldIdentifier id, bool is_vm_field) : Field(offset, std::move(id), is_vm_field) {}

//     OopField(InstanceKlass holder, int field_array_index) : Field(holder, field_array_index) {}

//     Oop get_value(const Oop &obj) const;

//     debugger::OopHandle get_value_as_oop_handle(const Oop &obj) const;

//     void set_value(Oop *obj) noexcept {}
// };

// class NarrowOopField : public OopField
// {
//   public:
//     using OopField::OopField;

//     NarrowOopField(std::shared_ptr<FieldIdentifier> id, uint64_t offset, bool isVMField)
//         : OopField(std::move(id), offset, isVMField)
//     {
//     }

//     template <typename VMField>
//     NarrowOopField(const VMField *vmField, uint64_t startOffset)
//         : OopField(std::make_shared<NamedFieldIdentifier>(vmField->getName()), vmField->getOffset() + startOffset,
//         true)
//     {
//     }

//     NarrowOopField(InstanceKlass *holder, int fieldArrayIndex) : OopField(holder, fieldArrayIndex) {}

//     // 压缩指针：先读 32bit narrowOop，再解码
//     Oop *getValue(const Oop *obj) const override;
//     OopHandle *getValueAsOopHandle(const Oop *obj) const override;
// };
} // namespace hotspot::oops

namespace std
{
template <> struct hash<hotspot::oops::FieldIdentifier>
{
    size_t operator()(const hotspot::oops::FieldIdentifier &id) const { return hotspot::oops::hash_code(id); }
};
} // namespace std