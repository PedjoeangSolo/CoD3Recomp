#include <jni.h>
#include <string>
#include "ppc_runtime_tests.h"

namespace cod3::ppc {
bool run_upstream_context_probe() noexcept;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_pedjoeangsolo_cod3recomp_MainActivity_nativeStatus(JNIEnv* env, jclass) {
    const bool memory_tests_ok = cod3::ppc::run_guest_memory_tests();
    const bool upstream_context_ok = cod3::ppc::run_upstream_context_probe();

    const std::string status =
        "Native library: LOADED\n"
        "JNI bridge: OK\n"
        "Target ABI: arm64-v8a\n"
        "XenonRecomp PPCContext compile: PASS\n"
        "XenonRecomp PPCContext layout probe: " + std::string(upstream_context_ok ? "PASS" : "FAIL") + "\n"
        "Guest memory prototype tests: " + std::string(memory_tests_ok ? "PASS" : "FAIL") + "\n\n"
        "Stage: Upstream PowerPC context ARM64 compatibility\n"
        "PPCContext alignment, vector register size, scalar/vector register access tested.\n"
        "Guest memory prototype tests big-endian access, bounds, and 32-bit wraparound.\n\n"
        "IMPORTANT: This is not a complete Xbox 360 runtime.\n"
        "Guest instruction execution, kernel imports, graphics/audio, and game boot are not implemented.";

    return env->NewStringUTF(status.c_str());
}
