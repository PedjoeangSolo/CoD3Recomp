#include "ppc_runtime_tests.h"
#include "ppc_guest_memory.h"
#include "ppc_cpu_state.h"
#include <cstdint>

namespace cod3::ppc {
bool run_guest_memory_tests() noexcept {
    try {
        GuestMemory mem(64, 0x1000);
        if (!mem.write16be(0x1000, 0x1234)) return false;
        if (!mem.write32be(0x1002, 0x89ABCDEFu)) return false;
        if (!mem.write64be(0x1008, 0x0123456789ABCDEFULL)) return false;
        std::uint16_t h = 0;
        std::uint32_t w = 0;
        std::uint64_t d = 0;
        if (!mem.read16be(0x1000, h) || h != 0x1234) return false;
        if (!mem.read32be(0x1002, w) || w != 0x89ABCDEFu) return false;
        if (!mem.read64be(0x1008, d) || d != 0x0123456789ABCDEFULL) return false;
        if (mem.contains(0x0FFF, 1) || mem.contains(0x1040, 1)) return false;
        if (mem.read32be(0x103F, w)) return false;
        CpuState state{};
        state.pc = 0x82000000u;
        state.gpr[3] = 0xA5A5u;
        return state.pc == 0x82000000u && state.gpr[3] == 0xA5A5u;
    } catch (...) {
        return false;
    }
}
}
