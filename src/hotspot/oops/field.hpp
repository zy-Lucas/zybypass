#pragma once

#include "instanceKlass.hpp"
#include "oop.hpp"
#include "symbol.hpp"
#include <cstdint>
#include <variant>

namespace hotspot::oops
{
class NamedFieldIdentifier
{
  public:
    NamedFieldIdentifier(std::string_view name) noexcept : name(name) {}

    std::string_view get_name_view() const noexcept { return name; }
    std::string get_name() const { return std::string{name}; }

    bool operator==(const NamedFieldIdentifier &o) const noexcept { return name == o.name; }

    size_t hash_code() const noexcept { return std::hash<std::string_view>{}(name); }

  private:
    std::string_view name;
};

class IndexableFieldIdentifier
{
  public:
    IndexableFieldIdentifier(uint32_t index) noexcept : index(index) {}

    uint32_t get_index() const noexcept { return index; }
    std::string get_name() const { return std::to_string(index); }

    bool operator==(const IndexableFieldIdentifier &o) const noexcept { return index == o.index; }

    size_t hash_code() const noexcept { return index; }

  private:
    uint32_t index;
};

using FieldIdentifier = std::variant<NamedFieldIdentifier, IndexableFieldIdentifier>;

inline std::string get_name(const FieldIdentifier &id)
{
    return std::visit([](const auto &v) { return v.get_name(); }, id);
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
    FieldType(Symbol signature) noexcept : signature(signature), first(signature.get_byte_at(0)) {}

    bool is_oop() const noexcept { return is_object() || is_array(); }
    bool is_byte() const noexcept { return first == 'B'; }
    bool is_char() const noexcept { return first == 'C'; }
    bool is_double() const noexcept { return first == 'D'; }
    bool is_float() const noexcept { return first == 'F'; }
    bool is_int() const noexcept { return first == 'I'; }
    bool is_long() const noexcept { return first == 'J'; }
    bool is_short() const noexcept { return first == 'S'; }
    bool is_boolean() const noexcept { return first == 'Z'; }
    bool is_object() const noexcept { return first == 'L'; }
    bool is_array() const noexcept { return first == '['; }

    Symbol get_signature() const noexcept { return signature; }

    struct ArrayInfo
    {
        int32_t dimension;
        int32_t element_basic_type;
    };
    ArrayInfo get_array_info() const noexcept;

  private:
    static void skip_optional_size(Symbol sig, uint32_t &index) noexcept;

    Symbol signature;
    uint8_t first;
};

class Field
{
  public:
    Field(uint64_t offset, FieldIdentifier id, bool is_vm_field);
    Field(InstanceKlass holder, uint32_t field_index);

    uint64_t get_offset() const noexcept { return offset; }
    const FieldIdentifier &get_id() const noexcept { return id; }
    Symbol get_name() const noexcept { return name; }

    bool is_vm_field() const noexcept { return vm_field; }
    bool is_named_field() const noexcept { return std::holds_alternative<NamedFieldIdentifier>(id); }

    uint32_t get_field_index() const noexcept { return field_index; }
    InstanceKlass get_field_holder() const noexcept { return holder; }

    uint64_t get_access_flags() const noexcept { return access_flags.value(); };
    runtime::AccessFlags get_access_flags_obj() const noexcept { return access_flags; };

    FieldType get_field_type() const noexcept { return field_type; }

    Symbol get_signature() const noexcept { return signature; }
    Symbol get_generic_signature() const noexcept { return generic_signature; }

    bool has_initial_value() const noexcept { return holder.get_field_initial_value_index(field_index); }

    bool is_public() const noexcept { return access_flags.is_public(); }
    bool is_private() const noexcept { return access_flags.is_private(); }
    bool is_protected() const noexcept { return access_flags.is_protected(); }
    bool is_package_private() const noexcept { return !is_public() && !is_private() && !is_protected(); }

    bool is_static() const noexcept { return access_flags.is_static(); }
    bool is_final() const noexcept { return access_flags.is_final(); }
    bool is_volatile() const noexcept { return access_flags.is_volatile(); }
    bool is_transient() const noexcept { return access_flags.is_transient(); }

    bool is_synthetic() const noexcept { return access_flags.is_synthetic(); }
    bool is_enum_constant() const noexcept { return access_flags.is_enum(); }

    bool operator==(const Field &other) const;

    size_t hash_code() const noexcept;

  private:
    Symbol name{0};
    uint64_t offset;
    FieldIdentifier id;
    bool vm_field = false;

    InstanceKlass holder{0};
    Symbol generic_signature{0};
    Symbol signature{0};
    FieldType field_type{0};
    runtime::AccessFlags access_flags{0};
    uint32_t field_index = 0;
};

#define DECLARE_PRIMITIVE_FIELD(ClassName, Type)                                                                       \
    class ClassName : public Field                                                                                     \
    {                                                                                                                  \
      public:                                                                                                          \
        ClassName(uint64_t offset, FieldIdentifier id, bool is_vm_field)                                               \
            : Field(offset, std::move(id), is_vm_field) {}                                                             \
                                                                                                                       \
        ClassName(InstanceKlass holder, uint32_t field_array_index) : Field(holder, field_array_index) {}              \
                                                                                                                       \
        Type get_value(const Oop &obj) const noexcept                                                                  \
        {                                                                                                              \
            return runtime::Jvm::read<Type>(obj.get_handle().address() + get_offset());                                \
        }                                                                                                              \
                                                                                                                       \
        void set_value(const Oop &obj, Type value) noexcept                                                            \
        {                                                                                                              \
            runtime::Jvm::write(obj.get_handle().address() + get_offset(), value);                                     \
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