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
        vertexBuffer.create(vertexCount);
    }
    shader = common::defaultShader;
}

std::size_t VertexArray::getVertexCount() const {
    return vertexBuffer.getVertexCount();
}

PrimitiveType VertexArray::getPrimitiveType() const {
    return vertexBuffer.getPrimitiveType();
}

Vertex& VertexArray::operator[](std::size_t index) {
    return vertexBuffer[index];
}

const Vertex& VertexArray::operator[](std::size_t index) const {
    return vertexBuffer[index];
}

void VertexArray::clear() {
    vertexBuffer.clear();
}

void VertexArray::resize(unsigned int newSize) {
    vertexBuffer.resize(newSize);
}

void VertexArray::append(const Vertex& vertex) {
    vertexBuffer.append(vertex);
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

void VertexArray::render(const Matrix4& view, const Matrix4& projection) const {
    if (shader == nullptr) return;
    auto modelMatrix = getModelMatrix();
    shader->use();
    shader->setMat4("model", modelMatrix);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    vertexBuffer.syncBuffer();
    renderBase(shader, texture, view, projection);
}

const VertexBuffer& VertexArray::getVertexBuffer() const {
    return vertexBuffer;
}

Vector2f VertexArray::getBoundsMin() const {
    // TODO: Implement bounds calculation
    return Vector2f(0.0f, 0.0f);
}

Vector2f VertexArray::getBoundsMax() const {
    // TODO: Implement bounds calculation
    return Vector2f(0.0f, 0.0f);
}

Matrix4 VertexArray::getModelMatrix() const {
    return Matrix4(); // Identity
}

}
