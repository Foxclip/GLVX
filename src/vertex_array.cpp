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

VertexArray::VertexArray(PrimitiveType type, std::size_t vertexCount) : m_vertex_buffer(type, Usage::StreamDraw) {
    if (vertexCount > 0) {
        m_vertices.resize(vertexCount);
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

void VertexArray::resize(unsigned int newSize) {
    m_vertices.resize(newSize);
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

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();

    for (const auto& vertex : m_vertices) {
        if (vertex.position.x < minX) minX = vertex.position.x;
        if (vertex.position.y < minY) minY = vertex.position.y;
    }

    return Vector2f(minX, minY);
}

Vector2f VertexArray::getBoundsMax() const {
    if (m_vertices.empty()) {
        return Vector2f(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    }

    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& vertex : m_vertices) {
        if (vertex.position.x > maxX) maxX = vertex.position.x;
        if (vertex.position.y > maxY) maxY = vertex.position.y;
    }

    return Vector2f(maxX, maxY);
}

Transform VertexArray::getTransform() const {
    return Transform();
}

}
