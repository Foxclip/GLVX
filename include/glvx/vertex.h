#pragma once

#include "glvx/vector.h"
#include "glvx/color.h"

namespace glvx {

struct Vertex {
    Vector2f position;
    Color color;
    Vector2f texCoords;
};

enum class PrimitiveType {
    Points,
    Lines,
    LineStrip,
    LineLoop,
    Triangles,
    TriangleStrip,
    TriangleFan
};

}
