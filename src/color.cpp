#include "glvis/color.h"

namespace glvis {

ColorRGB::ColorRGB(float r, float g, float b) {
    this->r = static_cast<std::uint8_t>(r * 255.0f);
    this->g = static_cast<std::uint8_t>(g * 255.0f);
    this->b = static_cast<std::uint8_t>(b * 255.0f);
}

std::string ColorRGB::toString(const ColorRGB& color) {
    return "("
        + std::to_string(color.r) + " "
        + std::to_string(color.g) + " "
        + std::to_string(color.b)
    + ")";
}

bool ColorRGB::operator==(const ColorRGB& other) const {
    return r == other.r && g == other.g && b == other.b;
}

ColorRGBA::ColorRGBA(float r, float g, float b, float a) {
    this->r = static_cast<std::uint8_t>(r * 255.0f);
    this->g = static_cast<std::uint8_t>(g * 255.0f);
    this->b = static_cast<std::uint8_t>(b * 255.0f);
    this->a = static_cast<std::uint8_t>(a * 255.0f);
}

std::string ColorRGBA::toString(const ColorRGBA& color) {
    return "("
        + std::to_string(color.r) + " "
        + std::to_string(color.g) + " "
        + std::to_string(color.b) + " "
        + std::to_string(color.a)
    + ")";
}

bool ColorRGBA::operator==(const ColorRGBA& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

const ColorRGB ColorRGB::White(1.0f, 1.0f, 1.0f);
const ColorRGB ColorRGB::Black(0.0f, 0.0f, 0.0f);
const ColorRGB ColorRGB::Red(1.0f, 0.0f, 0.0f);
const ColorRGB ColorRGB::Green(0.0f, 1.0f, 0.0f);
const ColorRGB ColorRGB::Blue(0.0f, 0.0f, 1.0f);

const ColorRGBA ColorRGBA::White(1.0f, 1.0f, 1.0f);
const ColorRGBA ColorRGBA::Black(0.0f, 0.0f, 0.0f);
const ColorRGBA ColorRGBA::Red(1.0f, 0.0f, 0.0f);
const ColorRGBA ColorRGBA::Green(0.0f, 1.0f, 0.0f);
const ColorRGBA ColorRGBA::Blue(0.0f, 0.0f, 1.0f);

}
