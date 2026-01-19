#include "glvis/color.h"

namespace glvis {

Color::Color(int r, int g, int b, int a) {
    this->r = static_cast<std::uint8_t>(r);
    this->g = static_cast<std::uint8_t>(g);
    this->b = static_cast<std::uint8_t>(b);
    this->a = static_cast<std::uint8_t>(a);
}

std::string Color::toString(const Color& color) {
    return "("
        + std::to_string(color.r) + " "
        + std::to_string(color.g) + " "
        + std::to_string(color.b) + " "
        + std::to_string(color.a)
    + ")";
}

bool Color::operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

const Color Color::White(255, 255, 255);
const Color Color::Black(0, 0, 0);
const Color Color::Red(255, 0, 0);
const Color Color::Green(0, 255, 0);
const Color Color::Blue(0, 0, 255);

}
