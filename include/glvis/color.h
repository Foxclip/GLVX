#pragma once

#include <cstdint>
#include <string>

namespace glvis {

class ColorRGB {
public:
    static const ColorRGB White;
    static const ColorRGB Black;
    static const ColorRGB Red;
    static const ColorRGB Green;
    static const ColorRGB Blue;

    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;

    ColorRGB() = default;
    ColorRGB(float r, float g, float b);
    static std::string toString(const ColorRGB& color);
    bool operator==(const ColorRGB& other) const;
};

class ColorRGBA {
public:
    static const ColorRGBA White;
    static const ColorRGBA Black;
    static const ColorRGBA Red;
    static const ColorRGBA Green;
    static const ColorRGBA Blue;

    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    ColorRGBA() = default;
    ColorRGBA(float r, float g, float b, float a = 1.0f);
    static std::string toString(const ColorRGBA& color);
    bool operator==(const ColorRGBA& other) const;
};

}
