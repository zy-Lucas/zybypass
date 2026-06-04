#include "javaTools.hpp"
#include "classfile/classLoaderDataGraph.hpp"
#include "code/codeCache.hpp"
#include "code/nmethod.hpp"
#include "hotspot/classfile/javaClasses.hpp"
#include "interpreter/interpreter.hpp"
#include "jni.h"
#include "oops/constantPool.hpp"
#include "oops/instanceKlass.hpp"

std::string JavaTools::to_std_string(jstring string) noexcept
{
    return hotspot::classfile::java_lang_String::to_std_string(hotspot::oops::Instance(*(uint64_t *)string));
}

void JavaTools::set_method_to_native(jstring klass_name, jstring method_name, jstring method_sign,
                                     uint64_t native_addr) noexcept
{
    hotspot::oops::InstanceKlass ik{hotspot::classfile::ClassLoaderDataGraph::find(to_std_string(klass_name))};
    int32_t index = ik.find_method_index(to_std_string(method_name), to_std_string(method_sign));
    hotspot::oops::Method method{ik.methods().at(index)};
    std::cout << method.name().as_view() << std::endl;
    uint64_t new_method = (uint64_t)malloc(method.size() + 2 * sizeof(void *));
    memcpy((void *)new_method, (void *)method.address(), method.size());
    memcpy((void *)(new_method + method.size()), &native_addr, sizeof(void *));
    hotspot::oops::Method new_met{new_method};
    new_met.set_is_native(true);
    new_met.set_from_interpreter_entry(hotspot::interpreter::Interpreter::code()
                                           .find_method_entry_point(hotspot::code::EntryPoint::native)
                                           .code_begin());
    replace_method(ik, method.address(), index, new_method);
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

    deoptimization_method(old_method);

    auto f = [old_method, new_method](hotspot::oops::Klass cb) {
        if (cb.is_instance_klass())
        {
            hotspot::oops::InstanceKlass ik{cb};
            ik.constants().cache().adjust_method_entries(old_method, new_method);
        }
    };
    hotspot::classfile::ClassLoaderDataGraph::classes_do(f);
}

void JavaTools::deoptimization_method(hotspot::oops::Method method) noexcept
{
    pthread_jit_write_protect_np(0);
    if (hotspot::code::nmethod nm{method.native_method()}; nm)
        nm.make_not_entrant();
    pthread_jit_write_protect_np(1);

    auto f = [method](hotspot::code::CodeBlob cb) {
        pthread_jit_write_protect_np(0);
        hotspot::code::nmethod nm{cb.address()};
        if (nm.is_alive() && nm.contains_method(method))
        {
            nm.make_not_entrant();
            std::cout << nm.method().name().as_view() << std::endl;
        }
        pthread_jit_write_protect_np(1);
    };

    hotspot::code::CodeCache::iterator_nmethods(f);
}