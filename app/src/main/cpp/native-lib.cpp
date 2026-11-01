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
        "Guest CPU state: INITIALIZED\n"
        "Guest memory tests: " + std::string(tests_ok ? "PASS" : "FAIL") + "\n\n"
        "Stage: PowerPC guest memory foundation\n"
        "Big-endian 16/32/64-bit access and bounds checks tested.\n\n"
        "STATUS: RUNTIME FOUNDATION ONLY\n"
        "PowerPC instruction execution, Xbox kernel, renderer, and game boot are not implemented yet.";
    return env->NewStringUTF(status.c_str());
}
