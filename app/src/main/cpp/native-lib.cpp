#include <jni.h>
#include <string>
#include "ppc_runtime_tests.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_pedjoeangsolo_cod3recomp_MainActivity_nativeStatus(JNIEnv* env, jclass) {
    const bool tests_ok = cod3::ppc::run_guest_memory_tests();
    const std::string status =
        "Native library: LOADED\n"
        "JNI bridge: OK\n"
        "Target ABI: arm64-v8a\n"
        "PPC state prototype: INITIALIZED\n"
        "Guest memory prototype tests: " + std::string(tests_ok ? "PASS" : "FAIL") + "\n\n"
        "Stage: Android ARM64 compatibility foundation\n"
        "Big-endian scalar access, bounds checks, and 32-bit address-wrap checks tested.\n\n"
        "IMPORTANT: This prototype is not XenonRecomp's PPCContext or a game runtime.\n"
        "PowerPC execution, Xbox kernel, graphics/audio, and game boot are not implemented.";
    return env->NewStringUTF(status.c_str());
}
