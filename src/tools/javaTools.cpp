#include "javaTools.hpp"
#include "classfile/classLoaderDataGraph.hpp"
#include "code/codeCache.hpp"
#include "hotspot/classfile/javaClasses.hpp"
#include "interpreter/interpreter.hpp"
#include "oops/instanceKlass.hpp"
#include "oops/klassVtable.hpp"

std::string JavaTools::to_std_string(jstring string) noexcept
{
    return hotspot::classfile::java_lang_String::to_std_string(hotspot::oops::Instance(*(uint64_t *)string));
}

std::pair<hotspot::oops::Method, int32_t> JavaTools::lookup_method(jstring klass_name, jstring method_name,
                                                                   jstring method_sign)
{
    std::string name{to_std_string(method_name)};
    std::string sig{to_std_string(method_sign)};

    std::cout << "addr: " << hotspot::oops::InstanceKlass{hotspot::classfile::ClassLoaderDataGraph::find(to_std_string(klass_name))}.address() << std::endl;

    for (hotspot::oops::InstanceKlass ik{hotspot::classfile::ClassLoaderDataGraph::find(to_std_string(klass_name))}; ik;
         ik = ik.super())
    {
        if (int32_t i = ik.find_method_index(name, sig); i >= 0)
            return {ik.methods().at(i), i};

        hotspot::oops::klassItable itable{ik};
        uint64_t n = itable.size_method_table() / sizeof(hotspot::oops::itableMethodEntry);

        for (uint32_t index = 0; index < n; ++index)
        {
            hotspot::oops::Method m{itable.method_entry(index)->method()};
            if (!m.name().equals(name) || !m.signature().equals(sig))
                continue;

            hotspot::oops::InstanceKlass holder{m.constants().pool_holder()};
            if (int32_t i = holder.find_method_index(name, sig); i >= 0) // find the method index
                return {m, i};
        }
    }
    return {0, -1};
}

void JavaTools::set_method_to_native(jstring klass_name, jstring method_name, jstring method_sign,
                                     uint64_t native_addr) noexcept
{
    const auto &[method, index]{lookup_method(klass_name, method_name, method_sign)};
    if (index < 0)
        return;
    std::cout << hotspot::oops::InstanceKlass{hotspot::classfile::ClassLoaderDataGraph::find(
                     to_std_string(klass_name))}.name().as_view()
              << std::endl;
    hotspot::oops::InstanceKlass ik{method.constants().pool_holder()};

    deoptimization_method(method);

    uint64_t new_method = (uint64_t)malloc(method.size() + 2 * sizeof(void *));
    memcpy((void *)new_method, (void *)method.address(), method.size());
    memcpy((void *)(new_method + method.size()), &native_addr, sizeof(void *));
    hotspot::oops::Method new_met{new_method};
    new_met.set_is_native(true);
    new_met.set_from_interpreter_entry(hotspot::interpreter::Interpreter::code()
                                           .find_method_entry_point(hotspot::code::EntryPoint::native)
                                           .code_begin());
    replace_method(ik, method, index, new_met);

    // std::cout << "nmethod: " << method.native_method().address() << std::endl;
    // std::cout << "dep: " << ik.dep_context().address() << std::endl;
    // for (hotspot::code::nmethodBucket nb{ik.dep_context()}; nb; nb = nb.next())
    //     std::cout << "nmethod: " << nb.nmethod().method().name().as_view() << " count: " << nb.count() << std::endl;
}

void JavaTools::replace_method(hotspot::oops::InstanceKlass ik, hotspot::oops::Method old_method, int32_t index,
                               hotspot::oops::Method new_method) noexcept
{
    hotspot::oops::Method::change_method_associated_with_jmethod_id(old_method.find_jmethod_id_or_null(), new_method);

    ik.methods().set_at(index, new_method.address());

    old_method.set_is_obsolete(true);
    if (uint16_t num = ik.next_method_idnum(); num != 0xFFFF)
        old_method.set_method_idnum(num);
    old_method.set_is_old(true);

    ik.vtable().replace_method(old_method, new_method);
    ik.itable().replace_method(old_method, new_method);

    auto f = [old_method, new_method](hotspot::oops::Klass cb) {
        if (cb.is_array_klass())
            cb.vtable().adjust_default_methods(old_method, new_method);
        else if (cb.is_instance_klass())
        {
            hotspot::oops::InstanceKlass ik{cb};

            hotspot::utilities::MethodArray methods{ik.methods()};
            int32_t num_methods = methods.length();
            for (uint32_t index = 0; index < num_methods; ++index)
                if (hotspot::oops::MethodData md{methods.at(index).method_data()}; md)
                    md.clean_extra_data();

            ik.vtable().adjust_default_methods(old_method, new_method);
            ik.adjust_default_methods(old_method, new_method);

            ik.itable().replace_method(old_method, new_method);

            ik.constants().cache().adjust_method_entries(old_method, new_method);
        }
    };
    hotspot::classfile::ClassLoaderDataGraph::classes_do(f);

    deoptimization_method(old_method);
}

void JavaTools::deoptimization_method(hotspot::oops::Method method) noexcept
{
    pthread_jit_write_protect_np(0);
    if (hotspot::code::nmethod nm{method.native_method()}; nm)
        nm.make_not_entrant();
    pthread_jit_write_protect_np(1);

    auto f = [method](hotspot::code::CodeBlob cb) {
        pthread_jit_write_protect_np(0);
        if (hotspot::code::nmethod nm{cb.address()};
            nm.is_alive() && !nm.is_marked_for_deoptimization() && nm.contains_method(method))
            nm.make_not_entrant();
        pthread_jit_write_protect_np(1);
    };

    hotspot::code::CodeCache::iterator_nmethods(f);
}