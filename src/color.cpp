#include "glvis/color.h"

namespace glvis {

    Color::Color() {
        r = 0;
        g = 0;
        b = 0;
        a = 255;
    }

    Color::Color(float r, float g, float b, float a) {
        this->r = static_cast<std::uint8_t>(r * 255.0f);
        this->g = static_cast<std::uint8_t>(g * 255.0f);
        this->b = static_cast<std::uint8_t>(b * 255.0f);
        this->a = static_cast<std::uint8_t>(a * 255.0f);
    }

}
