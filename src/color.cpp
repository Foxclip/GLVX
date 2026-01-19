#include "glvis/color.h"

namespace glvis {

ColorRGBA::ColorRGBA(int r, int g, int b, int a) {
    this->r = static_cast<std::uint8_t>(r);
    this->g = static_cast<std::uint8_t>(g);
    this->b = static_cast<std::uint8_t>(b);
    this->a = static_cast<std::uint8_t>(a);
}

ColorRGBA::ColorRGBA(std::initializer_list<int> list) {
    if (list.size() == 3) {
        auto it = list.begin();
        this->r = static_cast<std::uint8_t>(*it++);
        this->g = static_cast<std::uint8_t>(*it++);
        this->b = static_cast<std::uint8_t>(*it);
        this->a = 255;
    } else if (list.size() == 4) {
        auto it = list.begin();
        this->r = static_cast<std::uint8_t>(*it++);
        this->g = static_cast<std::uint8_t>(*it++);
        this->b = static_cast<std::uint8_t>(*it++);
        this->a = static_cast<std::uint8_t>(*it);
    } else {
        this->r = 0;
        this->g = 0;
        this->b = 0;
        this->a = 255;
    }
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

const ColorRGBA ColorRGBA::White(1.0f, 1.0f, 1.0f);
const ColorRGBA ColorRGBA::Black(0.0f, 0.0f, 0.0f);
const ColorRGBA ColorRGBA::Red(1.0f, 0.0f, 0.0f);
const ColorRGBA ColorRGBA::Green(0.0f, 1.0f, 0.0f);
const ColorRGBA ColorRGBA::Blue(0.0f, 0.0f, 1.0f);

}
