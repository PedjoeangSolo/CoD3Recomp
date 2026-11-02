// Compile-time/runtime layout probe for the upstream XenonRecomp PPC context.
// This deliberately does not execute guest instructions or touch the FP control register.
#define PPC_CONFIG_H_INCLUDED
#include "ppc_context.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

static_assert(alignof(PPCContext) == 0x40, "Unexpected PPCContext alignment");
static_assert(sizeof(PPCVRegister) == 16, "Unexpected PPC vector register size");
static_assert(std::is_standard_layout_v<PPCRegister>, "PPCRegister must remain standard-layout");

namespace cod3::ppc {

bool run_upstream_context_probe() noexcept {
    PPCContext context{};
    context.r3.u64 = 0x1122334455667788ull;
    context.v0.u8[0] = 0x12;
    context.v0.u8[15] = 0xFE;
    context.fpscr.csr = 0;

    return context.r3.u64 == 0x1122334455667788ull
        && context.v0.u8[0] == 0x12
        && context.v0.u8[15] == 0xFE
        && (reinterpret_cast<std::uintptr_t>(&context) & 0x3Fu) == 0;
}

} // namespace cod3::ppc
