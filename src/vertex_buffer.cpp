#include "glvis/vertex_buffer.h"
#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/vertex.h"

namespace glvis {

VertexBuffer::VertexBuffer(PrimitiveType type) : VertexBuffer(type, Usage::StaticDraw) { }

VertexBuffer::VertexBuffer(Usage usage) : VertexBuffer(PrimitiveType::Triangles, usage) { }

VertexBuffer::VertexBuffer(PrimitiveType type, Usage usage): VBO(0), VAO(0) {
    this->type = type;
    this->usage = usage;
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
    this->vertexCount = vertexCount;
    GL_CALL(glGenVertexArrays(1, &VAO));
    recreateBuffer(vertexCount);
    return true;
}

std::size_t VertexBuffer::getVertexCount() const {
    return vertexCount;
}

bool VertexBuffer::update(const std::vector<Vertex>& newVertices) {
    if (newVertices.size() != vertexCount) {
        vertexCount = newVertices.size();
        recreateBuffer(vertexCount);
    }
    if (isInitialized) {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, newVertices.size() * sizeof(Vertex), newVertices.data()));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
    return true;
}

bool VertexBuffer::update(const std::vector<Vertex>& newVertices, std::size_t vertexCount, unsigned int offset) {
    if (offset + vertexCount > this->vertexCount) {
        return false;
    }
    if (isInitialized) {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(Vertex), vertexCount * sizeof(Vertex), newVertices.data()));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
    return true;
}

void VertexBuffer::recreateBuffer(std::size_t size) {
    if (VBO != 0) {
        GL_CALL(glDeleteBuffers(1, &VBO));
        VBO = 0;
    }
    GL_CALL(glGenBuffers(1, &VBO));
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLenum usageGLenum;
    switch (usage) {
        case Usage::StaticDraw: usageGLenum = GL_STATIC_DRAW; break;
        case Usage::DynamicDraw: usageGLenum = GL_DYNAMIC_DRAW; break;
        case Usage::StreamDraw: usageGLenum = GL_STREAM_DRAW; break;
        default: throw std::invalid_argument("Invalid usage hint");
    }
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), nullptr, usageGLenum));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color)));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)));
    GL_CALL(glEnableVertexAttribArray(2));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindVertexArray(0));
    isInitialized = true;
    gpuBuffferSize = size;
}

PrimitiveType VertexBuffer::getPrimitiveType() const {
    return type;
}

void VertexBuffer::setPrimitiveType(PrimitiveType type) {
    this->type = type;
}

void VertexBuffer::setUsage(Usage usage) {
    this->usage = usage;
}

unsigned int VertexBuffer::getVAO() const {
    return VAO;
}

void VertexBuffer::render(const Matrix4& view, const Matrix4& projection) const {
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glDrawArrays(static_cast<GLenum>(type), 0, static_cast<GLsizei>(getVertexCount())));
}

}
