#include "glvx/vertex_buffer.h"
#include <stdexcept>
#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvx/vertex.h"

namespace glvx {

VertexBuffer::VertexBuffer(PrimitiveType type) : VertexBuffer(type, Usage::StaticDraw) { }

VertexBuffer::VertexBuffer(Usage usage) : VertexBuffer(PrimitiveType::Triangles, usage) { }

VertexBuffer::VertexBuffer(PrimitiveType type, Usage usage): VBO(0), VAO(0) {
    this->type = type;
    this->usage = usage;
}

VertexBuffer::~VertexBuffer() {
    if (!has_active_gl_context()) {
        return;
    }
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
    if (VAO == 0) {
        return true;
    }
    recreateBuffer(vertexCount);
    return true;
}

std::size_t VertexBuffer::getVertexCount() const {
    return vertexCount;
}

void VertexBuffer::ensureInitialized(std::size_t size) {
    if (size == 0) {
        return;
    }
    if (VAO == 0) {
        GL_CALL(glGenVertexArrays(1, &VAO));
    }
    recreateBuffer(size);
}

bool VertexBuffer::update(const std::vector<Vertex>& newVertices) {
    ensureInitialized(newVertices.size());
    if (newVertices.size() != vertexCount) {
        vertexCount = newVertices.size();
        recreateBuffer(vertexCount);
    }
    updateBuffer(newVertices.data(), 0, newVertices.size() * sizeof(Vertex));
    return true;
}

bool VertexBuffer::update(const std::vector<Vertex>& newVertices, std::size_t vertexCount, unsigned int offset) {
    if (offset + vertexCount > this->vertexCount) {
        return false;
    }
    updateBuffer(newVertices.data(), offset * sizeof(Vertex), vertexCount * sizeof(Vertex));
    return true;
}

void VertexBuffer::updateBuffer(const void* data, unsigned int offset, std::size_t size) {
    if (VBO == 0) {
        return;
    }
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
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
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0)));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color))));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords))));
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

void VertexBuffer::render() const {
    if (vertexCount == 0) {
        return;
    }
    if (VAO == 0) {
        return;
    }
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glDrawArrays(static_cast<GLenum>(type), 0, static_cast<GLsizei>(getVertexCount())));
}

}
