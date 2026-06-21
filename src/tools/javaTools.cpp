#include "javaTools.hpp"
#include "classfile/classLoaderDataGraph.hpp"
#include "code/codeCache.hpp"
#include "hotspot/classfile/javaClasses.hpp"
#include "interpreter/interpreter.hpp"
#include "oops/field.hpp"
#include "oops/klassVtable.hpp"
#include <algorithm>

std::string JavaTools::to_std_string(jstring string) noexcept
{
    return hotspot::classfile::java_lang_String::to_std_string(hotspot::oops::Instance(*(uint64_t *)string));
}

std::pair<hotspot::oops::Method, int32_t> JavaTools::lookup_method(std::string_view klass_name,
                                                                   std::string_view method_name,
                                                                   std::string_view method_sign)
{
    for (hotspot::oops::InstanceKlass ik{hotspot::classfile::ClassLoaderDataGraph::find(klass_name)}; ik;
         ik = ik.super())
    {
        if (int32_t i = ik.find_method_index(method_name, method_sign); i >= 0)
            return {ik.methods().at(i), i};

        hotspot::oops::klassItable itable{ik};
        uint64_t n = itable.size_method_table() / sizeof(hotspot::oops::itableMethodEntry);

        for (uint32_t index = 0; index < n; ++index)
        {
            hotspot::oops::Method m{itable.method_entry(index)->method()};
            if (!m.name().equals(method_name) || !m.signature().equals(method_sign))
                continue;

            hotspot::oops::InstanceKlass holder{m.constants().pool_holder()};
            if (int32_t i = holder.find_method_index(method_name, method_sign); i >= 0) // find the method index
                return {m, i};
        }
    }
    return {0, -1};
}

void JavaTools::set_method_to_native(jstring klass_name, jstring method_name, jstring method_sign, uint64_t native_addr,
                                     JNIEnv *env) noexcept
{
    set_method_to_native(to_std_string(klass_name), to_std_string(method_name), to_std_string(method_sign), native_addr,
                         env);
}

void JavaTools::set_method_to_native(const std::string &klass_name, const std::string &method_name,
                                     const std::string &method_sign, uint64_t native_addr, JNIEnv *env) noexcept
{
    const auto &[method, index]{lookup_method(klass_name, method_name, method_sign)};
    if (index < 0)
        return;
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
    if (env)
        adjust_method_handle_entry(env, klass_name, method_name, method_sign, new_met);
    replace_method(ik, method, index, new_met);
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
            cb.vtable().adjust_default_method(old_method, new_method);
        else if (cb.is_instance_klass())
        {
            hotspot::oops::InstanceKlass ik{cb};

            hotspot::utilities::MethodArray methods{ik.methods()};
            int32_t num_methods = methods.length();
            for (uint32_t index = 0; index < num_methods; ++index)
                if (hotspot::oops::MethodData md{methods.at(index).method_data()}; md)
                    md.clean_extra_data();

            ik.vtable().adjust_default_method(old_method, new_method);
            ik.adjust_default_method(old_method, new_method);

            ik.itable().replace_method(old_method, new_method);

            ik.constants().cache().adjust_method_entry(old_method, new_method);
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

jstring toJString(JNIEnv *env, const std::string &str)
{
    if (str.empty())
        return env->NewStringUTF("");
    jstring jstr = env->NewStringUTF(str.c_str());
    if (env->ExceptionCheck())
        return nullptr;
    return jstr;
}

jclass JavaTools::find_class(JNIEnv *env, const std::string &slash_name)
{
    std::string class_name{slash_name};
    std::replace(class_name.begin(), class_name.end(), '/', '.');

    jobject thread = env->CallStaticObjectMethod(g_cache.thread_class, g_cache.currentThread);
    jobject classLoader = env->CallObjectMethod(thread, g_cache.getContextClassLoader);

    jstring str_name = toJString(env, class_name);
    jclass target =
        (jclass)env->CallStaticObjectMethod(g_cache.class_class, g_cache.forName, str_name, JNI_FALSE, classLoader);

    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        target = nullptr;
    }

    env->DeleteLocalRef(str_name);
    env->DeleteLocalRef(classLoader);
    env->DeleteLocalRef(thread);

    return target;
}

static int32_t ref_kind_for_method(hotspot::oops::Method method)
{
    if (!method)
        return 0;

    if (method.name().equals("<init>"))
        return 8; // REF_newInvokeSpecial

    hotspot::runtime::AccessFlags flags{method.access_flags_obj()};

    if (flags.is_static())
        return 6; // REF_invokeStatic
    if (flags.is_private())
        return 7; // REF_invokeSpecial
    if (method.constants().pool_holder().is_interface())
        return 9; // REF_invokeInterface
    return 5;     // REF_invokeVirtual
}

void JavaTools::adjust_method_handle_entry(JNIEnv *env, const std::string &klass_name, const std::string &method_name,
                                           const std::string &method_sign, hotspot::oops::Method new_method)
{
    init(env);

    jobject method_type =
        env->CallStaticObjectMethod(g_cache.mt_class, g_cache.from_desc, toJString(env, method_sign), nullptr);

    jclass target_class = find_class(env, klass_name.c_str());
    if (!target_class)
        return;

    int32_t ref_kind = ref_kind_for_method(new_method);

    jobject member_name_obj = env->NewObject(g_cache.mn_class, g_cache.ctor, target_class, toJString(env, method_name),
                                             method_type, ref_kind);

    jobject factory_obj = env->GetStaticObjectField(g_cache.factory_class, g_cache.instance_field);
    member_name_obj =
        env->CallObjectMethod(factory_obj, g_cache.resolve, ref_kind, member_name_obj, target_class, -1, true);
    jobject resolved_method_obj = env->GetObjectField(member_name_obj, g_cache.method_field);

    if (resolved_method_obj)
    {

        jclass objCls = env->FindClass("java/lang/Object");
        jmethodID toString = env->GetMethodID(objCls, "toString", "()Ljava/lang/String;");
        jstring str = (jstring)env->CallObjectMethod(resolved_method_obj, toString);
        const char *cstr = env->GetStringUTFChars(str, nullptr);
        std::cout << "Resolved method: " << cstr << std::endl;
        env->ReleaseStringUTFChars(str, cstr);
        env->DeleteLocalRef(str);
        env->DeleteLocalRef(objCls);

        hotspot::oops::Instance resolved_obj{*(uint64_t *)resolved_method_obj};
        hotspot::oops::InstanceKlass resolved_klass(resolved_obj.klass());

        static uint32_t vmtarget_field_index = resolved_klass.find_field("vmtarget", "J").field_index();
        hotspot::oops::LongField vmtarget_field{resolved_klass, vmtarget_field_index};

        vmtarget_field.set_value(resolved_obj, new_method.address());

        env->DeleteLocalRef(resolved_method_obj);
    }

    env->DeleteLocalRef(method_type);
    env->DeleteLocalRef(target_class);
    env->DeleteLocalRef(member_name_obj);
    env->DeleteLocalRef(factory_obj);
}

void JavaTools::init(JNIEnv *env)
{
    if (g_cache.mt_class)
        return;

    jclass local;

    local = env->FindClass("java/lang/invoke/MethodType");
    g_cache.mt_class = (jclass)env->NewGlobalRef(local);
    g_cache.from_desc =
        env->GetStaticMethodID(g_cache.mt_class, "fromMethodDescriptorString",
                               "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/invoke/MethodType;");

    local = env->FindClass("java/lang/invoke/MemberName");
    g_cache.mn_class = (jclass)env->NewGlobalRef(local);
    g_cache.ctor = env->GetMethodID(g_cache.mn_class, "<init>",
                                    "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;B)V");

    local = env->FindClass("java/lang/invoke/MemberName$Factory");
    g_cache.factory_class = (jclass)env->NewGlobalRef(local);
    g_cache.instance_field =
        env->GetStaticFieldID(g_cache.factory_class, "INSTANCE", "Ljava/lang/invoke/MemberName$Factory;");
    g_cache.resolve =
        env->GetMethodID(g_cache.factory_class, "resolve",
                         "(BLjava/lang/invoke/MemberName;Ljava/lang/Class;IZ)Ljava/lang/invoke/MemberName;");

    local = env->FindClass("java/lang/invoke/ResolvedMethodName");
    g_cache.resolved_method_class = (jclass)env->NewGlobalRef(local);
    g_cache.method_field = env->GetFieldID(g_cache.mn_class, "method", "Ljava/lang/invoke/ResolvedMethodName;");

    local = env->FindClass("java/lang/Class");
    g_cache.class_class = (jclass)env->NewGlobalRef(local);
    g_cache.forName = env->GetStaticMethodID(g_cache.class_class, "forName",
                                             "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;");

    local = env->FindClass("java/lang/Thread");
    g_cache.thread_class = (jclass)env->NewGlobalRef(local);
    g_cache.currentThread = env->GetStaticMethodID(g_cache.thread_class, "currentThread", "()Ljava/lang/Thread;");
    g_cache.getContextClassLoader =
        env->GetMethodID(g_cache.thread_class, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
}