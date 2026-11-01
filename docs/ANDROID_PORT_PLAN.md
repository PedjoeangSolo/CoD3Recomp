# Android ARM64 Port Plan

Target: Android 16, ARM64, initially tested on TECNO Pova 7 4G.

This branch is an experimental porting workspace. The upstream host is currently Windows-only and uses Win32 APIs, Direct3D 11, DXGI, WinMM, Windows codecs/dialogs, and Windows-specific build flags. An Android APK cannot be produced by changing the ABI alone.

## Non-negotiable constraints

- Build through GitHub Actions with the Android NDK; no Android Studio is required.
- Target `arm64-v8a` only for the first device-specific milestone.
- Do not commit an ISO, `default.xex`, extracted game assets, or generated game code derived from the user's game image.
- Keep the upstream `main` branch unchanged; all port work belongs on `android/arm64-port`.
- Do not describe a CI smoke-test APK as a playable game.

## Porting milestones

1. **Android build foundation:** add a minimal Android NativeActivity/JNI application and a GitHub Actions workflow that builds an ARM64 APK. This verifies the NDK, Gradle/CMake integration, packaging, and artifact delivery only.
2. **Host abstraction:** replace Win32 windowing, file selection, filesystem, timing, threading, audio output, and input with Android-compatible implementations.
3. **Graphics backend:** replace the Direct3D 11/DXGI executor with Vulkan (preferred) or another Android-supported backend. Preserve the existing guest GPU command translation where possible; do not assume HLSL can run directly on Android.
4. **Guest/runtime portability:** remove x86-64-only compiler flags and intrinsics, audit PowerPC guest endianness and memory mapping, and make generated guest code compile for AArch64.
5. **Private game build:** generate guest code from the user's own `default.xex` in a private workflow or local process. Never publish game-derived output.
6. **Device validation:** test startup, rendering, controls, audio, stability, and performance on the target TECNO device.

## Current known blockers

- `CoD3Host/CMakeLists.txt` explicitly fails configuration when `WIN32` is false.
- The host links Windows-only libraries including `d3d11`, `dxgi`, `winmm`, `ole32`, `dbghelp`, `windowscodecs`, `shell32`, and `winhttp`.
- The host source list includes a D3D11 renderer and Win32 ImGui backend.
- `CoD3RecompLib/ppc/` is generated and is not included in the source repository; the user's own XEX is required to generate it.
- The repository's existing recompile script is PowerShell/Windows-toolchain oriented.
- The XenonRecomp submodule must be initialized recursively in CI.

## Definition of the first CI milestone

A successful build produces an installable `arm64-v8a` APK that opens a diagnostic screen and reports that the Android native library loaded. It does not boot Call of Duty 3 yet.
