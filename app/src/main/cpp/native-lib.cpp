#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_pedjoeangsolo_cod3recomp_MainActivity_nativeStatus(JNIEnv* env, jclass) {
    const std::string status =
        "Native library: LOADED\n"
        "JNI bridge: OK\n"
        "Target ABI: arm64-v8a\n"
        "Build stage: Android foundation\n\n"
        "Next work: Android host APIs, Vulkan renderer, "
        "and AArch64 guest runtime.";
    return env->NewStringUTF(status.c_str());
}
