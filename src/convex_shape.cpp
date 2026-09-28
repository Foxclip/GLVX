#include "glvx/convex_shape.h"
#include <cassert>

namespace glvx {

std::size_t ConvexShape::requiredVertexCount(std::size_t point_count) {
    if (point_count < 3) {
        return 0;
    }
    // + 2 for the center vertex and the repeated first point
    return point_count + 2;
}

ConvexShape::ConvexShape(std::size_t point_count)
    : Shape(PrimitiveType::TriangleFan,
    static_cast<unsigned int>(requiredVertexCount(point_count))),
    m_points(point_count) {
    updateVertices();
}

ConvexShape::~ConvexShape() { }

void ConvexShape::setPointCount(std::size_t point_count) {
    m_points.resize(point_count);
    updateVertices();
}

std::size_t ConvexShape::getPointCount() const {
    return m_points.size();
}

void ConvexShape::setPoint(std::size_t index, const Vector2f& point) {
    assert(index < m_points.size());
    m_points[index] = point;
    updateVertices();
}

const Vector2f& ConvexShape::getPoint(std::size_t index) const {
    assert(index < m_points.size());
    return m_points[index];
}

void ConvexShape::updateVertices() {
    const std::size_t point_count = m_points.size();
    const std::size_t required = requiredVertexCount(point_count);

    // Only resize (and push to the GPU) when the vertex count actually changed,
    // e.g. via setPointCount(). The constructor pre-allocates the buffer.
    if (m_vertices.size() != required) {
        resize(static_cast<unsigned int>(required));
    }

    if (required == 0) {
        return;
    }

    Vector2f bounds_min = m_points[0];
    Vector2f bounds_max = m_points[0];

    for (const auto& point : m_points) {
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

    // The first vertex is the center of the shape
    const Vector2f center = (bounds_min + bounds_max) / 2.0f;
    Vertex& center_vertex = getVertex(0);
    center_vertex.position = center;
    center_vertex.color = Color::White;
    center_vertex.tex_coords = Vector2f(
        (center.x - bounds_min.x) / span.x,
        (center.y - bounds_min.y) / span.y
    );

    for (std::size_t i = 0; i < point_count; i++) {
        Vertex& vertex = getVertex(i + 1);
        vertex.position = m_points[i];
        vertex.color = Color::White;
        vertex.tex_coords = Vector2f(
            (m_points[i].x - bounds_min.x) / span.x,
            (m_points[i].y - bounds_min.y) / span.y
        );
    }

    // Repeat the first point to close the fan
    getVertex(point_count + 1) = getVertex(1);
}

}
