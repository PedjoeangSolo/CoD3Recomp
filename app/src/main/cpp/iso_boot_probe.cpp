#include <jni.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <set>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>

namespace {
constexpr uint64_t kSector = 2048;
constexpr char kMagic[] = "MICROSOFT*XBOX*MEDIA";
constexpr size_t kMagicLen = 20;
struct Entry { uint32_t sector; uint32_t size; uint8_t attributes; std::string name; };

uint16_t u16(const uint8_t* p) {
    return uint16_t(p[0]) | (uint16_t(p[1]) << 8);
}
uint32_t u32(const uint8_t* p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) |
           (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}
bool read_at(int fd, uint64_t offset, void* data, size_t size, uint64_t file_size) {
    if (offset > file_size || size > file_size - offset) return false;
    auto* p = static_cast<uint8_t*>(data);
    size_t done = 0;
    while (done < size) {
        const ssize_t n = pread(fd, p + done, size - done, static_cast<off_t>(offset + done));
        if (n <= 0) return false;
        done += static_cast<size_t>(n);
    }
    return true;
}
bool iequals(const std::string& a, const char* b) {
    if (a.size() != std::strlen(b)) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) return false;
    return true;
}
bool walk_dir(int fd, uint64_t base, uint64_t file_size, uint32_t sector,
              uint32_t size, int depth, std::set<uint64_t>& visited,
              std::string& found, uint64_t& xex_size, std::string& error) {
    if (depth > 48) { error = "XDVDFS directory nesting limit exceeded"; return false; }
    if (size == 0 || size > (16u << 20)) { error = "Invalid XDVDFS directory size"; return false; }
    std::vector<uint8_t> table(size);
    const uint64_t dir_offset = base + uint64_t(sector) * kSector;
    if (dir_offset < base || !read_at(fd, dir_offset, table.data(), size, file_size)) {
        error = "Cannot read XDVDFS directory table"; return false;
    }
    std::vector<uint16_t> pending{0};
    std::vector<Entry> subdirs;
    while (!pending.empty()) {
        const uint16_t word = pending.back(); pending.pop_back();
        const uint64_t byte = uint64_t(word) * 4;
        if (byte + 14 > size || !visited.insert((uint64_t(sector) << 32) | word).second) continue;
        const uint8_t* e = table.data() + byte;
        const uint16_t left = u16(e), right = u16(e + 2);
        Entry item{u32(e + 4), u32(e + 8), e[12], {}};
        const uint8_t name_len = e[13];
        if (name_len == 0 || byte + 14 + name_len > size) continue;
        item.name.assign(reinterpret_cast<const char*>(e + 14), name_len);
        if (item.attributes & 0x10) {
            subdirs.push_back(item);
        } else if (iequals(item.name, "default.xex")) {
            const uint64_t xoff = base + uint64_t(item.sector) * kSector;
            uint8_t magic[4]{};
            if (!read_at(fd, xoff, magic, sizeof(magic), file_size) ||
                std::memcmp(magic, "XEX2", 4) != 0) {
                error = "Found default.xex entry, but its header is not XEX2";
                return false;
            }
            found = item.name;
            xex_size = item.size;
            return true;
        }
        if (left != 0 && left != 0xffff) pending.push_back(left);
        if (right != 0 && right != 0xffff) pending.push_back(right);
    }
    for (const auto& d : subdirs) {
        if (walk_dir(fd, base, file_size, d.sector, d.size, depth + 1,
                     visited, found, xex_size, error)) return true;
        if (!error.empty()) return false;
    }
    return false;
}
} // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_com_pedjoeangsolo_cod3recomp_MainActivity_nativeInspectIso(
        JNIEnv* env, jclass, jint fd) {
    struct stat st{};
    if (fd < 0 || fstat(fd, &st) != 0 || st.st_size <= 0) {
        return env->NewStringUTF("ERROR: Cannot read selected ISO file descriptor.");
    }
    const uint64_t file_size = static_cast<uint64_t>(st.st_size);
    struct Format { uint64_t offset; const char* name; };
    constexpr Format formats[] = {
        {0x00000000ull, "stripped/rebuilt"},
        {0x02080000ull, "XGD3"},
        {0x0FD90000ull, "XGD2"},
        {0x18300000ull, "XGD1"}
    };
    uint8_t sector[kSector]{};
    for (const auto& fmt : formats) {
        const uint64_t descriptor = fmt.offset + 32 * kSector;
        if (!read_at(fd, descriptor, sector, sizeof(sector), file_size)) continue;
        if (std::memcmp(sector, kMagic, kMagicLen) != 0 ||
            std::memcmp(sector + kSector - kMagicLen, kMagic, kMagicLen) != 0) continue;
        const uint32_t root_sector = u32(sector + 0x14);
        const uint32_t root_size = u32(sector + 0x18);
        std::set<uint64_t> visited;
        std::string found, error;
        uint64_t xex_size = 0;
        const bool ok = walk_dir(fd, fmt.offset, file_size, root_sector, root_size,
                                 0, visited, found, xex_size, error);
        if (!ok) {
            if (error.empty()) error = "XDVDFS found, but default.xex was not found.";
            return env->NewStringUTF(("ISO format: " + std::string(fmt.name) +
                "\n" + error + "\nNo game boot was attempted.").c_str());
        }
        const std::string result = "ISO: VALID XDVDFS\nFormat: " + std::string(fmt.name) +
            "\nExecutable: default.xex\nXEX header: XEX2 VALID\nXEX size: " +
            std::to_string(xex_size) + " bytes\n\nThis is ISO/XEX discovery only. " 
            "The Android Xbox 360 runtime is not yet implemented; game boot has not been attempted.";
        return env->NewStringUTF(result.c_str());
    }
    return env->NewStringUTF("ERROR: No supported Xbox 360 XDVDFS volume descriptor found. The file may be another ISO format, encrypted, truncated, or unsupported.");
}
