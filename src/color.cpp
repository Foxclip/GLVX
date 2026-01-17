#include "glvis/color.h"
#include "color.h"

namespace glvis {

ColorRGB::ColorRGB(float r, float g, float b) {
    this->r = static_cast<std::uint8_t>(r * 255.0f);
    this->g = static_cast<std::uint8_t>(g * 255.0f);
    this->b = static_cast<std::uint8_t>(b * 255.0f);
}

ColorRGBA::ColorRGBA(float r, float g, float b, float a) {
    this->r = static_cast<std::uint8_t>(r * 255.0f);
    this->g = static_cast<std::uint8_t>(g * 255.0f);
    this->b = static_cast<std::uint8_t>(b * 255.0f);
    this->a = static_cast<std::uint8_t>(a * 255.0f);
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
