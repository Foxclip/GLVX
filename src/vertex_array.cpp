#include "glvis/vertex_array.h"
#include <algorithm>
#include <limits>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/shader.h"
#include "glvis/glvis_common.h"

namespace glvis {

VertexArray::VertexArray() : vertexBuffer(Usage::StreamDraw) {
    shader = common::defaultShader;
}

VertexArray::VertexArray(PrimitiveType type, std::size_t vertexCount) : vertexBuffer(type, Usage::StreamDraw) {
    if (vertexCount > 0) {
        vertices.resize(vertexCount);
        vertexBuffer.create(vertexCount);
    }
    shader = common::defaultShader;
}

std::size_t VertexArray::getVertexCount() const {
    return vertices.size();
}

PrimitiveType VertexArray::getPrimitiveType() const {
    return vertexBuffer.getPrimitiveType();
}

Vertex& VertexArray::operator[](std::size_t index) {
    return vertices[index];
}

const Vertex& VertexArray::operator[](std::size_t index) const {
    return vertices[index];
}

void VertexArray::clear() {
    vertices.clear();
    vertexBuffer.create(0);
}

void VertexArray::resize(unsigned int newSize) {
    vertices.resize(newSize);
    if (vertices.empty()) {
        vertexBuffer.create(0);
    } else {
        vertexBuffer.update(vertices);
    }
}

void VertexArray::append(const Vertex& vertex) {
    vertices.push_back(vertex);
    vertexBuffer.update(vertices);
}

void VertexArray::setPrimitiveType(PrimitiveType type) {
    vertexBuffer.setPrimitiveType(type);
}

void VertexArray::setShader(Shader* shader) {
    this->shader = shader;
}

void VertexArray::setTexture(AbstractTexture* texture) {
    this->texture = texture;
}

void VertexArray::render(const Matrix4& view, const Matrix4& projection, const RenderStates& states) const {
    Shader* renderShader = states.shader ? states.shader : shader;
    if (renderShader == nullptr) return;
    AbstractTexture* renderTexture = states.texture ? states.texture : texture;
    renderShader->use();
    Matrix4 combinedModel = states.transform * getModelMatrix();
    vertexBuffer.update(vertices);
    renderBase(renderShader, renderTexture, color, combinedModel, view, projection);
}

const VertexBuffer& VertexArray::getVertexBuffer() const {
    return vertexBuffer;
}

Vector2f VertexArray::getBoundsMin() const {
    if (vertices.empty()) {
        return Vector2f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    }

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();

    for (const auto& vertex : vertices) {
        if (vertex.position.x < minX) minX = vertex.position.x;
        if (vertex.position.y < minY) minY = vertex.position.y;
    }

    return Vector2f(minX, minY);
}

Vector2f VertexArray::getBoundsMax() const {
    if (vertices.empty()) {
        return Vector2f(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    }

    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& vertex : vertices) {
        if (vertex.position.x > maxX) maxX = vertex.position.x;
        if (vertex.position.y > maxY) maxY = vertex.position.y;
    }

    return Vector2f(maxX, maxY);
}

Matrix4 VertexArray::getModelMatrix() const {
    return Matrix4(); // Identity
}

}
