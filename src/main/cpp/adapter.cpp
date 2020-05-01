#include <cstdlib>

#include "adapter.h"

// JNI generated code
#include "dev_luncliff_Module.h"
#include "dev_luncliff_Module2.h"

bool is_test_mode() { return ::getenv("TEST") != nullptr; }

// JNI callbacks
extern "C" {

/**
 * @see https://lsit81.tistory.com/entry/JNIOnLoad를-이용한-Interface-정의
 */
jint JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env{};
    jint result = -1;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }
    ::fputs(__func__, stdout);
    return JNI_VERSION_1_6;
}

/**
 * @see http://bleaklow.com/2006/02/18/jni_onunload_mostly_useless.html
 */
void JNI_OnUnload(JavaVM* vm, void*) {
    ::fputs(__func__, stdout);
    return;
}

} // extern "C"

jboolean Java_dev_luncliff_Module_isTestMode(JNIEnv*, jclass) {
    return is_test_mode();
}

jboolean Java_dev_luncliff_Module2_isTestMode(JNIEnv* env, jclass clazz) {
    return is_test_mode();
}
