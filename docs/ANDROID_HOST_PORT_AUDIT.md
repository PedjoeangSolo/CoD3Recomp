# Android Host Port Audit

This document records the boundary between the existing Windows host and the Android ARM64 target. It is an engineering inventory, not a claim that the game boots on Android.

## Verified Android foundation

- Gradle packages an `arm64-v8a` APK.
- JNI loads the native library.
- The upstream XenonRecomp `PPCContext` header compiles for Android ARM64.
- A runtime probe checks the upstream context alignment, scalar register access, vector register width/access, and the guest-memory prototype tests.
- The upstream ARM64 FPCR operand-width adjustment is applied in the CI checkout only; it does not rewrite the upstream submodule commit.

## Existing host code that is not Android-ready

The existing `CoD3Host` implementation is Windows-specific. Its kernel implementation includes Win32 APIs and Windows exception/context handling. Other host areas use Windows file/window/audio/rendering facilities. The existing `CoD3RecompLib` build also sets an x86-64 compiler baseline and must not be compiled unchanged for ARM64.

The import table is useful as a checklist, but an import name appearing in the table does not imply that its implementation is portable or correct on Android.

## Porting boundaries

### 1. Guest address space and memory
- Replace Windows virtual-memory reservation/commit and exception-driven demand paging with Android/Linux `mmap`, `mprotect`, and explicit page management.
- Preserve Xbox 360 guest 32-bit virtual addresses and the title's physical-memory windows.
- Preserve PPC big-endian load/store semantics and MMIO routing.
- Do not assume the existing small guest-memory test buffer is the final 4 GiB guest address-space implementation.
- Validate address reservation availability and page protections on real ARM64 devices before attempting title boot.

### 2. Threading and kernel objects
- Port guest threads, events, semaphores, mutants, waits, TLS, APC and critical sections to pthreads/Android primitives.
- Preserve guest-visible scheduling and synchronization semantics; a simple one-to-one stub is not sufficient.
- Keep missing imports fatal and diagnostic rather than returning fabricated success values.

### 3. Filesystem and XEX loading
- Implement a user-selected private game directory using Android's Storage Access Framework or app-private storage.
- Parse/load the XEX and its sections/metadata in the Android process.
- Keep game files and generated title-specific code out of the public repository and CI artifacts.
- Reconcile the title's expected guest paths with Android's scoped-storage rules.

### 4. Graphics and GPU
- Replace D3D11/DXGI and Win32 window/swap-chain code with an Android surface plus Vulkan (preferred where supported) or OpenGL ES.
- Implement guest GPU register/MMIO behavior, command processor, EDRAM/tile behavior, shader translation, fences and presentation. A blank Android surface is not a renderer.
- Determine the actual device GPU and Vulkan/OpenGL ES capabilities before choosing backend details.

### 5. Audio and input
- Replace XAudio/XMA host integration with Android audio output and an XMA decode path.
- Map XInput-style controller state to Android touch/gamepad input without changing guest-visible structures.

## Ordered implementation gates

1. Keep the upstream PPCContext ARM64 probe passing.
2. Build an isolated Android guest-memory/address-space module and test mapping, permissions, endian access, MMIO ranges, and wraparound.
3. Port a minimal set of kernel primitives with deterministic unit tests.
4. Add XEX loading from private app storage and validate headers/sections without executing title code.
5. Generate title code privately from the user's own XEX, then compile a tiny generated-code fixture against the Android runtime ABI.
6. Add GPU/audio/input services incrementally.
7. Attempt title boot only after imports and MMIO/GPU requirements are sufficiently implemented; report the first unsupported import or instruction precisely.

## Current non-goals

- No claim of game boot or playable performance.
- No emulator, Wine, or Winlator dependency.
- No public upload of ISO, XEX, generated title code, or proprietary game assets.
