from pathlib import Path

header = Path("tools/XenonRecomp/XenonUtils/ppc_context.h")
if not header.is_file():
    raise SystemExit(f"XenonRecomp header not found: {header}")

source = header.read_text(encoding="utf-8")
old = '''    inline void setcsr(uint32_t csr) noexcept
    {
        __asm__ __volatile__("msr fpcr, %0" : : "r"(csr));
    }'''
new = '''    inline void setcsr(uint32_t csr) noexcept
    {
        // AArch64 MSR FPCR requires an X-register operand (64-bit).
        const uint64_t host_csr = static_cast<uint64_t>(csr);
        __asm__ __volatile__("msr fpcr, %0" : : "r"(host_csr));
    }'''
if new in source:
    print("XenonRecomp ARM64 FPCR operand patch already applied.")
elif old in source:
    header.write_text(source.replace(old, new, 1), encoding="utf-8")
    print("Applied local ARM64 FPCR operand-width compatibility patch.")
else:
    raise SystemExit("Expected XenonRecomp setcsr() code not found; refusing unsafe patch.")
