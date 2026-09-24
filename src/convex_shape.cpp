#include "glvx/convex_shape.h"
#include <cassert>

namespace glvx {

ConvexShape::ConvexShape(std::size_t point_count) : Shape(PrimitiveType::TriangleFan, point_count) {
    updateVertices();
}

ConvexShape::~ConvexShape() {
}

void ConvexShape::setPointCount(std::size_t point_count) {
    resize(static_cast<unsigned int>(point_count));
    updateVertices();
}

std::size_t ConvexShape::getPointCount() const {
    return getVertexCount();
}

void ConvexShape::setPoint(std::size_t index, const Vector2f& point) {
    assert(index < getVertexCount());
    getVertex(index).position = point;
    updateVertices();
}

const Vector2f& ConvexShape::getPoint(std::size_t index) const {
    assert(index < getVertexCount());
    return getVertex(index).position;
}

void ConvexShape::updateVertices() {
    const std::size_t count = getVertexCount();
    if (count == 0) {
        return;
    }

    Vector2f bounds_min = getVertex(0).position;
    Vector2f bounds_max = getVertex(0).position;

    for (std::size_t i = 0; i < count; i++) {
        const Vector2f& point = getVertex(i).position;
        if (point.x < bounds_min.x) {
            bounds_min.x = point.x;
        }
        if (point.y < bounds_min.y) {
            bounds_min.y = point.y;
        }
        if (point.x > bounds_max.x) {
            bounds_max.x = point.x;
        }
        if (point.y > bounds_max.y) {
            bounds_max.y = point.y;
        }
    }

    Vector2f span = bounds_max - bounds_min;
    if (span.x == 0.0f) {
        span.x = 1.0f;
    }
    if (span.y == 0.0f) {
        span.y = 1.0f;
    }

    for (std::size_t i = 0; i < count; i++) {
        Vertex& vertex = getVertex(i);
        vertex.color = Color::White;
        vertex.tex_coords = Vector2f(
            (vertex.position.x - bounds_min.x) / span.x,
            (vertex.position.y - bounds_min.y) / span.y
        );
    }
}

}
