#pragma once

#include <cassert>
#include <cstdint>
#include <string>

namespace glvx {

class Color {
public:
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    static const Color& White;
    static const Color& Black;
    static const Color& Red;
    static const Color& Green;
    static const Color& Blue;
    static const Color& Yellow;
    static const Color& Transparent;

    constexpr Color() = default;
    // Defined (not just declared) in this header so that it can be
    // constant-evaluated in every translation unit — the detail::color_*
    // storage below is constant-initialized in each TU that uses it.
    constexpr Color(int r, int g, int b, int a = 255)
        : r(static_cast<std::uint8_t>(r)), g(static_cast<std::uint8_t>(g)), b(static_cast<std::uint8_t>(b)), a(static_cast<std::uint8_t>(a)) {
        assert(r >= 0 && r <= 255);
        assert(g >= 0 && g <= 255);
        assert(b >= 0 && b <= 255);
        assert(a >= 0 && a <= 255);
    }

    static std::string toString(const Color& color);
    bool operator==(const Color& other) const;
};

namespace detail {
// Constant-initialized: values are baked in before any dynamic initialization
inline constexpr Color color_white{255, 255, 255, 255};
inline constexpr Color color_black{0, 0, 0, 255};
inline constexpr Color color_red{255, 0, 0, 255};
inline constexpr Color color_green{0, 255, 0, 255};
inline constexpr Color color_blue{0, 0, 255, 255};
inline constexpr Color color_yellow{255, 255, 0, 255};
inline constexpr Color color_transparent{0, 0, 0, 0};
}

inline const Color& Color::White{detail::color_white};
inline const Color& Color::Black{detail::color_black};
inline const Color& Color::Red{detail::color_red};
inline const Color& Color::Green{detail::color_green};
inline const Color& Color::Blue{detail::color_blue};
inline const Color& Color::Yellow{detail::color_yellow};
inline const Color& Color::Transparent{detail::color_transparent};

}
