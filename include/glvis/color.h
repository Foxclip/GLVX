#pragma once

#include <cstdint>
#include <string>

namespace glvis {

class Color {
public:
    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;

    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    Color() = default;
    Color(int r, int g, int b, int a = 255);
    static std::string toString(const Color& color);
    bool operator==(const Color& other) const;
};

}
