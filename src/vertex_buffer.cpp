#include "glvis/vertex_buffer.h"
#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/vertex.h"

namespace glvis {

VertexBuffer::VertexBuffer() {
    VBO = 0;
    VAO = 0;
}

VertexBuffer::~VertexBuffer() {
    if (VAO != 0) {
        GL_CALL(glDeleteVertexArrays(1, &VAO));
        VAO = 0;
    }
    if (VBO != 0) {
        GL_CALL(glDeleteBuffers(1, &VBO));
        VBO = 0;
    }
}

bool VertexBuffer::create(std::size_t vertexCount) {
    vertices.resize(vertexCount);
    GL_CALL(glGenVertexArrays(1, &VAO));
    return syncBuffer();
}

std::size_t VertexBuffer::getVertexCount() const {
    return vertices.size();
}

bool VertexBuffer::update(const std::vector<Vertex>& newVertices) {
    vertices = newVertices;
    return syncBuffer();
}

bool VertexBuffer::resize(std::size_t newSize) {
    vertices.resize(newSize);
    return syncBuffer();
}

bool VertexBuffer::append(const Vertex& vertex) {
    vertices.push_back(vertex);
    return syncBuffer();
}

bool VertexBuffer::syncBuffer() {
    if (isInitialized && vertices.size() == gpuBuffferSize) {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    } else {
        recreateBuffer(vertices);
    }
    return true;
}

void VertexBuffer::recreateBuffer(const std::vector<Vertex>& data) {
    if (VBO != 0) {
        GL_CALL(glDeleteBuffers(1, &VBO));
        VBO = 0;
    }
    GL_CALL(glGenBuffers(1, &VBO));
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex), data.data(), GL_STATIC_DRAW));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color)));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)));
    GL_CALL(glEnableVertexAttribArray(2));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindVertexArray(0));
    isInitialized = true;
    gpuBuffferSize = data.size();
}

void VertexBuffer::clear() {
    update(std::vector<Vertex>());
}

PrimitiveType VertexBuffer::getPrimitiveType() const {
    return type;
}

void VertexBuffer::setPrimitiveType(PrimitiveType type) {
    this->type = type;
}

unsigned int VertexBuffer::getVAO() const {
    return VAO;
}

void VertexBuffer::render(const glm::mat4& view, const glm::mat4& projection) const {
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glDrawArrays(static_cast<GLenum>(type), 0, static_cast<GLsizei>(getVertexCount())));
}

Vertex& VertexBuffer::operator[](std::size_t index) {
    return vertices[index];
}

const Vertex& VertexBuffer::operator[](unsigned int index) const {
    return vertices[index];
}

}
