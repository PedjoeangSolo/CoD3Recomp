# XenonRecomp integration notes for Android ARM64

## Verified milestone

The Linux CI workflow successfully builds the upstream XenonRecomp host tools:

- `XenonRecomp`: converts an Xbox 360 XEX executable into C++.
- `XenonAnalyse`: analyzes an XEX and can emit jump-table metadata.

The workflow run is recorded at:
https://github.com/PedjoeangSolo/CoD3Recomp/actions/runs/41354275079

The uploaded `xenonrecomp-linux-tools` artifact contains host-side Linux executables. These tools are not Android binaries and are not part of the APK.

## Important architecture distinction

XenonRecomp is a static recompiler, not an emulator and not a complete runtime. It emits C++ that must be compiled together with a compatible game runtime. Its README explicitly states that making the game work requires a separate runtime implementation.

The current Android JNI library is only a diagnostic foundation. In particular:

- `ppc_cpu_state.h` contains a small experimental state prototype.
- It is **not ABI-compatible** with upstream `PPCContext`.
- `ppc_guest_memory.*` tests scalar big-endian access and address bounds; it is **not** the final 4-GiB guest base-memory mapping expected by generated code.
- The Android app does not execute generated PPC code and does not boot the game.

## Requirements before generated game code can be linked

1. **Use the exact XenonRecomp context contract.** Generated functions use `PPC_FUNC` signatures with a `PPCContext&` and a `uint8_t* base`. The context includes GPR/FPR, condition registers, XER, FPSCR, vector registers, and other state. Do not substitute the current prototype struct.
2. **Preserve guest memory semantics.** Upstream generated loads/stores use a base pointer and perform endian conversion in the generated instruction helpers. The final Android memory implementation must preserve those semantics, alignment assumptions, volatile accesses, and indirect-function lookup layout. Do not byte-swap the whole image as a shortcut.
3. **Validate ARM64 SIMD behavior.** Upstream documents ARM64 VMX support through SIMDe, including vector byte ordering and floating-point denormal handling. Build and test those paths using the Android NDK/Clang toolchain.
4. **Implement Xbox kernel imports and platform services.** Static code still depends on kernel imports, threads, synchronization, timers, files, memory management, audio, input, and other title-specific services.
5. **Replace the Windows host layer.** The upstream CoD3 host uses Windows APIs and Direct3D 11. Android needs an Android-native lifecycle/input/audio layer and a graphics backend such as Vulkan or OpenGL ES, plus shader/resource conversion.
6. **Keep title data private.** Do not commit the user's XEX, ISO, extracted assets, or generated title-specific C++ to this public repository. Run recompiler generation in a private/local environment and only publish code that is safe to redistribute.

## Next technical milestone

Create an isolated Android ARM64 compatibility target that compiles the upstream XenonUtils PPC context and SIMD support with the NDK, then add focused tests for register layout, scalar endian loads/stores, vector lane order, and FPSCR behavior. Only after that should title-specific generated C++ be introduced through a private build process.

## Current status

| Component | Status |
|---|---|
| Linux XenonRecomp/XenonAnalyse host tools | Built in GitHub Actions |
| Android arm64-v8a JNI diagnostic APK | Builds in GitHub Actions |
| Experimental guest-memory prototype | Basic tests; not final runtime memory |
| Upstream PPCContext integration | Not started |
| Generated CoD3 C++ | Not generated/integrated |
| Xbox kernel/runtime imports | Not implemented |
| Android renderer/audio/input host | Not implemented |
| Game boot | Not implemented |
