#include "glvx/vertex_array.h"
#include <algorithm>
#include <limits>
#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvx/shader.h"
#include "glvx/glvx_common.h"

namespace glvx {

VertexArray::VertexArray() : m_vertex_buffer(Usage::StreamDraw) { }

VertexArray::VertexArray(PrimitiveType type, std::size_t vertex_count) : m_vertex_buffer(type, Usage::StreamDraw) {
    if (vertex_count > 0) {
        m_vertices.resize(vertex_count);
    }
}

std::size_t VertexArray::getVertexCount() const {
    return m_vertices.size();
}

PrimitiveType VertexArray::getPrimitiveType() const {
    return m_vertex_buffer.getPrimitiveType();
}

Vertex& VertexArray::operator[](std::size_t index) {
    assert(index < m_vertices.size());
    return m_vertices[index];
}

const Vertex& VertexArray::operator[](std::size_t index) const {
    assert(index < m_vertices.size());
    return m_vertices[index];
}

Vertex& VertexArray::getVertex(std::size_t index) {
    assert(index < m_vertices.size());
    return m_vertices[index];
}

const Vertex& VertexArray::getVertex(std::size_t index) const {
    assert(index < m_vertices.size());
    return m_vertices[index];
}

void VertexArray::clear() {
    m_vertices.clear();
    m_vertex_buffer.create(0);
}

void VertexArray::resize(unsigned int new_size) {
    m_vertices.resize(new_size);
    if (m_vertices.empty()) {
        m_vertex_buffer.create(0);
    } else {
        m_vertex_buffer.update(m_vertices);
    }
}

void VertexArray::append(const Vertex& vertex) {
    m_vertices.push_back(vertex);
    m_vertex_buffer.update(m_vertices);
}

void VertexArray::setPrimitiveType(PrimitiveType type) {
    m_vertex_buffer.setPrimitiveType(type);
}

void VertexArray::render(const Matrix4& view, const Matrix4& projection, const RenderStates& states) const {
    START_TRY
    m_vertex_buffer.update(m_vertices);
    renderBase(m_shader, m_texture, m_color, getTransform(), view, projection, states);
    END_TRY
}

const VertexBuffer& VertexArray::getVertexBuffer() const {
    return m_vertex_buffer;
}

Vector2f VertexArray::getBoundsMin() const {
    if (m_vertices.empty()) {
        return Vector2f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    }

    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();

    for (const auto& vertex : m_vertices) {
        if (vertex.position.x < min_x) min_x = vertex.position.x;
        if (vertex.position.y < min_y) min_y = vertex.position.y;
    }

    return Vector2f(min_x, min_y);
}

Vector2f VertexArray::getBoundsMax() const {
    if (m_vertices.empty()) {
        return Vector2f(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    }

    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();

    for (const auto& vertex : m_vertices) {
        if (vertex.position.x > max_x) max_x = vertex.position.x;
        if (vertex.position.y > max_y) max_y = vertex.position.y;
    }

    return Vector2f(max_x, max_y);
}

Transform VertexArray::getTransform() const {
    return Transform();
}

}
