#pragma once

namespace glvx {

struct BlendMode {
    enum class Factor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha
    };

    enum class Equation {
        Add,
        Subtract,
        ReverseSubtract
    };

    Factor colorSrcFactor{Factor::One};
    Factor colorDstFactor{Factor::OneMinusSrcAlpha};
    Equation colorEquation{Equation::Add};
    Factor alphaSrcFactor{Factor::One};
    Factor alphaDstFactor{Factor::OneMinusSrcAlpha};
    Equation alphaEquation{Equation::Add};
};

bool operator==(const BlendMode& a, const BlendMode& b);
bool operator!=(const BlendMode& a, const BlendMode& b);

inline const BlendMode BlendDefault;
inline const BlendMode BlendAlpha{
    BlendMode::Factor::SrcAlpha,
    BlendMode::Factor::OneMinusSrcAlpha,
    BlendMode::Equation::Add,
    BlendMode::Factor::SrcAlpha,
    BlendMode::Factor::OneMinusSrcAlpha,
    BlendMode::Equation::Add
};
inline const BlendMode BlendAdd{
    BlendMode::Factor::One,
    BlendMode::Factor::One,
    BlendMode::Equation::Add,
    BlendMode::Factor::One,
    BlendMode::Factor::One,
    BlendMode::Equation::Add
};
inline const BlendMode BlendMultiply{
    BlendMode::Factor::DstColor,
    BlendMode::Factor::OneMinusSrcAlpha,
    BlendMode::Equation::Add,
    BlendMode::Factor::One,
    BlendMode::Factor::OneMinusSrcAlpha,
    BlendMode::Equation::Add
};
inline const BlendMode BlendNone{
    BlendMode::Factor::One,
    BlendMode::Factor::Zero,
    BlendMode::Equation::Add,
    BlendMode::Factor::One,
    BlendMode::Factor::Zero,
    BlendMode::Equation::Add
};

} // namespace glvx
