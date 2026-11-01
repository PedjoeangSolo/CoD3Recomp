#include "ppc_guest_memory.h"

#include <limits>

namespace cod3::ppc {
GuestMemory::GuestMemory(std::size_t size, std::uint32_t base)
    : base_(base), data_(size, 0) {}

bool GuestMemory::contains(std::uint32_t address, std::size_t length) const noexcept {
    if (address < base_) return false;

    // Guest addresses are 32-bit. Reject ranges that would wrap past 0xFFFFFFFF.
    const auto address_space_remaining =
        static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max()) -
        static_cast<std::uint64_t>(address) + 1ULL;
    if (static_cast<std::uint64_t>(length) > address_space_remaining) return false;

    const auto off = static_cast<std::uint64_t>(address) - base_;
    return off <= data_.size() &&
           length <= data_.size() - static_cast<std::size_t>(off);
}

std::size_t GuestMemory::offset(std::uint32_t address) const noexcept {
    return static_cast<std::size_t>(static_cast<std::uint64_t>(address) - base_);
}

bool GuestMemory::read8(std::uint32_t a, std::uint8_t& v) const noexcept {
    if (!contains(a, 1)) return false;
    v = data_[offset(a)];
    return true;
}
bool GuestMemory::read16be(std::uint32_t a, std::uint16_t& v) const noexcept {
    if (!contains(a, 2)) return false;
    const auto o = offset(a);
    v = (static_cast<std::uint16_t>(data_[o]) << 8) |
        static_cast<std::uint16_t>(data_[o + 1]);
    return true;
}
bool GuestMemory::read32be(std::uint32_t a, std::uint32_t& v) const noexcept {
    if (!contains(a, 4)) return false;
    const auto o = offset(a);
    v = (static_cast<std::uint32_t>(data_[o]) << 24) |
        (static_cast<std::uint32_t>(data_[o + 1]) << 16) |
        (static_cast<std::uint32_t>(data_[o + 2]) << 8) |
        static_cast<std::uint32_t>(data_[o + 3]);
    return true;
}
bool GuestMemory::read64be(std::uint32_t a, std::uint64_t& v) const noexcept {
    if (!contains(a, 8)) return false;
    const auto o = offset(a);
    std::uint64_t result = 0;
    for (std::size_t i = 0; i < 8; ++i) {
        result = (result << 8) | data_[o + i];
    }
    v = result;
    return true;
}
bool GuestMemory::write8(std::uint32_t a, std::uint8_t v) noexcept {
    if (!contains(a, 1)) return false;
    data_[offset(a)] = v;
    return true;
}
bool GuestMemory::write16be(std::uint32_t a, std::uint16_t v) noexcept {
    if (!contains(a, 2)) return false;
    const auto o = offset(a);
    data_[o] = static_cast<std::uint8_t>(v >> 8);
    data_[o + 1] = static_cast<std::uint8_t>(v);
    return true;
}
bool GuestMemory::write32be(std::uint32_t a, std::uint32_t v) noexcept {
    if (!contains(a, 4)) return false;
    const auto o = offset(a);
    data_[o] = static_cast<std::uint8_t>(v >> 24);
    data_[o + 1] = static_cast<std::uint8_t>(v >> 16);
    data_[o + 2] = static_cast<std::uint8_t>(v >> 8);
    data_[o + 3] = static_cast<std::uint8_t>(v);
    return true;
}
bool GuestMemory::write64be(std::uint32_t a, std::uint64_t v) noexcept {
    if (!contains(a, 8)) return false;
    const auto o = offset(a);
    for (std::size_t i = 0; i < 8; ++i) {
        const auto shift = static_cast<unsigned>((7 - i) * 8);
        data_[o + i] = static_cast<std::uint8_t>(v >> shift);
    }
    return true;
}
}
