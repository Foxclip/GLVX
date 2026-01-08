#include "vertex_array.h"
#include <algorithm>
#include <limits>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glvis {

    VertexArray::VertexArray() { }

    VertexArray::VertexArray(PrimitiveType type, std::size_t vertexCount) {
        vertexBuffer.create(vertexCount);
        vertexBuffer.setPrimitiveType(type);
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

    const Vertex& VertexArray::operator[](unsigned int index) const {
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

    void VertexArray::render(const glm::mat4& view, const glm::mat4& projection) const {
        vertexBuffer.render(view, projection);
    }

}
