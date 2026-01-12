#pragma once

#include "glvis/vector.h"
#include "glvis/color.h"

namespace glvis {

struct Vertex {
    Vector2 position;
    Color color;
    Vector2 texCoords;
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
