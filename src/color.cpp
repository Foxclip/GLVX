#include "glvx/color.h"
#include <cassert>

namespace glvx {

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

// NOTE: the Color::White & friends constants are defined inline in color.h as
// references to constant-initialized (constexpr) storage, on purpose: they must

}
