#pragma once

#include <cstdint>
#include <string>
#include <initializer_list>

namespace glvis {

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
    ColorRGBA(int r, int g, int b, int a = 255);
    ColorRGBA(std::initializer_list<int> list);
    explicit ColorRGBA(float r, float g, float b, float a = 1.0f);
    static std::string toString(const ColorRGBA& color);
    bool operator==(const ColorRGBA& other) const;
};

}
