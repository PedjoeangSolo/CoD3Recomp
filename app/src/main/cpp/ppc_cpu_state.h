#pragma once
#include <array>
#include <cstdint>
namespace cod3::ppc {
struct CpuState final {
    std::array<std::uint64_t, 32> gpr{};
    std::array<std::uint64_t, 32> fpr{};
    std::uint32_t pc = 0;
    std::uint32_t lr = 0;
    std::uint32_t ctr = 0;
    std::uint32_t xer = 0;
    std::uint32_t cr = 0;
    std::uint64_t fpscr = 0;
};
}
