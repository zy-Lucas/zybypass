#pragma once

#include "oops/instanceKlass.hpp"
#include "oops/method.hpp"

namespace hotspot::oops
{
class vtableEntry
{
  public:
    Method method() const noexcept { return method_; }
    void set_method(Method method) noexcept { method_ = method; }

  private:
    Method method_;
};

class itableMethodEntry
{
  public:
    Method method() const noexcept { return method_; }
    void set_method(Method method) noexcept { method_ = method; }

  private:
    Method method_;
};

class itableOffsetEntry
{
  public:
    InstanceKlass interface_klass() const noexcept { return interface_; }
    int32_t offset() const noexcept { return offset_; }

    itableMethodEntry *first_method_entry(Klass k) const noexcept { return method_entry(k, offset_); }

    static itableMethodEntry *method_entry(Klass k, int32_t offset) noexcept;

  private:
    InstanceKlass interface_;
    int32_t offset_;
};

class klassVtable
{
  public:
    klassVtable(Klass klass, uint64_t base, int32_t length) noexcept;

    vtableEntry *vtable() const noexcept { return (vtableEntry *)(klass_.address() + table_offset_); }

    void replace_method(Method old_method, Method new_method) noexcept;

    void adjust_default_methods(Method old_method, Method new_method) noexcept;

  private:
    Klass klass_;
    uint64_t table_offset_;
    int32_t length_;
};

class klassItable
{
  public:
    klassItable(InstanceKlass klass) noexcept;

    itableOffsetEntry *offset_entry(uint32_t index) const noexcept;
    itableMethodEntry *method_entry(uint32_t index) const noexcept;

    bool replace_method(Method old_method, Method new_method) noexcept;

    uint64_t size_offset_table() const noexcept { return size_offset_table_; }
    uint64_t size_method_table() const noexcept { return size_method_table_; }

  private:
    uint64_t vtable_start() const noexcept { return klass_.address() + table_offset_; }
    uint64_t method_start() const noexcept { return vtable_start() + size_offset_table_; }

    InstanceKlass klass_;
    uint64_t table_offset_;
    uint64_t size_offset_table_;
    uint64_t size_method_table_;
};
} // namespace hotspot::oops