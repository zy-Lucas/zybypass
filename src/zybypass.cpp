#include "hotspot/classfile/classLoaderDataGraph.hpp"
#include "hotspot/code/codeCache.hpp"
#include "hotspot/oops/field.hpp"
#include "render/overlay.h"
#include "tools/javaTools.hpp"

static JavaVM *g_javaVM = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    hotspot::runtime::Jvm::init();
    g_javaVM = vm;
    JNIEnv *env = nullptr;

    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_8) != JNI_OK)
    {
        return JNI_ERR;
    }

    return JNI_VERSION_1_8;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) { g_javaVM = nullptr; }

jfloat ex() { return 20; }

extern "C" void JNIEXPORT Java_net_zy_terminus_Main_putMethod(JNIEnv *env, jclass, jstring klass_name,
                                                                  jstring method_name, jstring method_sign)
{
    JavaTools::set_method_to_native(klass_name, method_name, method_sign, (uint64_t)ex, env);
    // const auto& [method, index]{JavaTools::lookup_method(klass_name, method_name, method_sign)};
    // env->CallStaticVoidMethod((jclass)method.constants().pool_holder().java_mirror().handle().address(),
    // (jmethodID)&method);
}

extern "C" void JNIEXPORT Java_net_zy_terminus_Main_resolved(JNIEnv *env, jclass, jobject resolved)
{
    // JavaTools::set_method_to_native("net/zy/terminus/Terminus", "putMethod",
    //                                 "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
    //                                 (uint64_t)Java_net_zy_terminus_Terminus_putMethod, env);
    if (resolved)
    {
        hotspot::oops::Instance resolved_obj{*(uint64_t *)resolved};
        hotspot::oops::InstanceKlass resolved_klass(resolved_obj.klass());

        static uint32_t vmtarget_field_index = resolved_klass.find_field("vmtarget", "J").field_index();
        hotspot::oops::LongField vmtarget_field{resolved_klass, vmtarget_field_index};
        std::cout << vmtarget_field.value(resolved_obj) << std::endl;
    }
}

extern "C" void JNIEXPORT Java_net_endofcosmos_sword_natives_Native_test(JNIEnv *env, jclass, jstring klass_name,
                                                                         jstring method_name, jstring method_sign)
{
    hotspot::oops::Method method =
        hotspot::oops::InstanceKlass{
            hotspot::classfile::ClassLoaderDataGraph::find(JavaTools::to_std_string(klass_name))}
            .find_method(JavaTools::to_std_string(method_name), JavaTools::to_std_string(method_sign));
    pthread_jit_write_protect_np(0);
    if (hotspot::code::nmethod nm(method.native_method()); nm)
    {
        nm.make_not_entrant();
        // memset((void *)nm.verified_entry_point(), 0, 16);
    }
    pthread_jit_write_protect_np(1);

    auto f = [method](hotspot::code::CodeBlob cb) {
        pthread_jit_write_protect_np(0);
        hotspot::code::nmethod nm{cb.address()};
        if (nm.method().name().equals("callFoo"))
        {
        }
        if (nm.is_alive() && nm.contains_method(method))
            nm.make_not_entrant();
        pthread_jit_write_protect_np(1);
    };

    hotspot::code::CodeCache::iterator_nmethods(f);
}

extern "C" void JNIEXPORT Java_net_endofcosmos_sword_natives_Native_startDeathRender(JNIEnv *env, jclass)
{
    StartOverlay();
}

extern "C" void JNIEXPORT Java_net_endofcosmos_sword_natives_Native_stopDeathRender(JNIEnv *env, jclass)
{
    StopOverlay();
}

// extern "C" void JNIEXPORT registration()
// {
//     JNIEnv *env = hotspot::runtime::Threads::current().jni_environment();

//     JavaTools::set_method_to_native("net/zy/terminus/Terminus", "putMethod",
//                                     "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
//                                     (uint64_t)Java_net_zy_terminus_Terminus_putMethod, env);
// }