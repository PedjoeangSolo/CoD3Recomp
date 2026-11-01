#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace cod3::ppc {
class GuestMemory final {
public:
    explicit GuestMemory(std::size_t size, std::uint32_t base = 0);
    bool contains(std::uint32_t address, std::size_t length) const noexcept;
    bool read8(std::uint32_t address, std::uint8_t& out) const noexcept;
    bool read16be(std::uint32_t address, std::uint16_t& out) const noexcept;
    bool read32be(std::uint32_t address, std::uint32_t& out) const noexcept;
    bool read64be(std::uint32_t address, std::uint64_t& out) const noexcept;
    bool write8(std::uint32_t address, std::uint8_t value) noexcept;
    bool write16be(std::uint32_t address, std::uint16_t value) noexcept;
    bool write32be(std::uint32_t address, std::uint32_t value) noexcept;
    bool write64be(std::uint32_t address, std::uint64_t value) noexcept;
private:
    std::size_t offset(std::uint32_t address) const noexcept;
    std::uint32_t base_;
    std::vector<std::uint8_t> data_;
};
}
