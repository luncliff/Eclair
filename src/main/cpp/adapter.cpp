
#include "cpp-gradle-in-kor.h"
#include "impl.h"

// JNI generated code
#include "dev_luncliff_Module.h"

extern "C" { // JNI native library callbacks

// https://lsit81.tistory.com/entry/JNIOnLoad를-이용한-Interface-정의
jint JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env{};
    jint result = -1;

    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }
    std::fputs(__func__, stdout);
    return JNI_VERSION_1_6;
}

// http://bleaklow.com/2006/02/18/jni_onunload_mostly_useless.html
void JNI_OnUnload(JavaVM* vm, void*) {
    std::fputs(__func__, stdout);
    return;
}
}

jboolean Java_dev_luncliff_Module_isTestMode(JNIEnv*, jclass) {
    return std::getenv("TEST") != nullptr;
}

jbyteArray Java_dev_luncliff_Module_makeBlob(JNIEnv* env, jclass, jint) {
    if (env == nullptr)
        env->GetByteArrayElements(nullptr, 0);
    return nullptr;
}
