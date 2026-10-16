#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

namespace ufmnext::core {

struct Color {
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};
    uint8_t a{255};

     Color() noexcept = default;
     Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) noexcept
        : r(red), g(green), b(blue), a(alpha) {}

    static Color FromHex(const std::string& hexStr) {
        std::string hex = hexStr;
        if (!hex.empty() && hex[0] == '#') {
            hex = hex.substr(1);
        }
        if (hex.length() == 6) {
            uint32_t val = 0;
            std::stringstream ss;
            ss << std::hex << hex;
            ss >> val;
            return Color(static_cast<uint8_t>((val >> 16) & 0xFF),
                         static_cast<uint8_t>((val >> 8) & 0xFF),
                         static_cast<uint8_t>(val & 0xFF),
                         255);
        } else if (hex.length() == 8) {
            uint32_t val = 0;
            std::stringstream ss;
            ss << std::hex << hex;
            ss >> val;
            return Color(static_cast<uint8_t>((val >> 16) & 0xFF),
                         static_cast<uint8_t>((val >> 8) & 0xFF),
                         static_cast<uint8_t>(val & 0xFF),
                         static_cast<uint8_t>((val >> 24) & 0xFF));
        }
        return Black();
    }

    std::string ToHex(bool includeAlpha = false) const {
        std::ostringstream ss;
        ss << "#";
        if (includeAlpha) {
            ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(a);
        }
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(r)
           << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(g)
           << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(b);
        return ss.str();
    }

     bool operator==(const Color& other) const noexcept {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

     bool operator!=(const Color& other) const noexcept {
        return !(*this == other);
    }

    static  Color Black() noexcept { return Color(0, 0, 0, 255); }
    static  Color White() noexcept { return Color(255, 255, 255, 255); }
    static  Color Red() noexcept { return Color(220, 53, 69, 255); }
    static  Color Green() noexcept { return Color(40, 167, 69, 255); }
    static  Color Blue() noexcept { return Color(0, 122, 255, 255); }
    static  Color Yellow() noexcept { return Color(255, 193, 7, 255); }
    static  Color Cyan() noexcept { return Color(23, 162, 184, 255); }
    static  Color Magenta() noexcept { return Color(232, 62, 140, 255); }
    static  Color Orange() noexcept { return Color(253, 126, 20, 255); }
    static  Color DarkGray() noexcept { return Color(52, 58, 64, 255); }
    static  Color Gray() noexcept { return Color(108, 117, 125, 255); }
    static  Color LightGray() noexcept { return Color(233, 236, 239, 255); }
    static  Color SoftBlue() noexcept { return Color(204, 229, 255, 255); }
    static  Color Transparent() noexcept { return Color(0, 0, 0, 0); }
};

} // namespace ufmnext::core
