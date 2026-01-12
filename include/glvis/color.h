#pragma once
#include <cstdint>

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

    Color();
    Color(float r, float g, float b, float a = 1.0f);

};

}
