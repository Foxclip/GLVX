#pragma once

#include "glvis/vector.h"
#include "glvis/color.h"

namespace glvis {

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
