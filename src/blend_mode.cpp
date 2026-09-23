#include "glvx/blend_mode.h"

namespace glvx {

bool operator==(const BlendMode& a, const BlendMode& b) {
    return a.colorSrcFactor == b.colorSrcFactor &&
           a.colorDstFactor == b.colorDstFactor &&
           a.colorEquation == b.colorEquation &&
           a.alphaSrcFactor == b.alphaSrcFactor &&
           a.alphaDstFactor == b.alphaDstFactor &&
           a.alphaEquation == b.alphaEquation;
}

bool operator!=(const BlendMode& a, const BlendMode& b) {
    return !(a == b);
}

} // namespace glvx
