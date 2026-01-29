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


VertexBuffer::VertexBuffer(PrimitiveType type) {
    this->type = type;
    this->usage = Usage::StaticDraw;
    VBO = 0;
    VAO = 0;
}


VertexBuffer::VertexBuffer(Usage usage) {
    this->type = PrimitiveType::Triangles;
    this->usage = usage;
    VBO = 0;
    VAO = 0;
}


VertexBuffer::VertexBuffer(PrimitiveType type, Usage usage) {
    this->type = type;
    this->usage = usage;
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
    if (isInitialized && vertices.size() == gpuBuffferSize) {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    } else {
        recreateBuffer(vertices);
    }
    return true;
}

std::size_t VertexBuffer::getVertexCount() const {
    return vertices.size();
}

bool VertexBuffer::update(const std::vector<Vertex>& newVertices) {
    vertices = newVertices;
    if (isInitialized && vertices.size() == gpuBuffferSize) {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    } else {
        recreateBuffer(vertices);
    }
    return true;
}

bool VertexBuffer::update(const Vertex* newVertices, std::size_t vertexCount, unsigned int offset) {
    if (offset + vertexCount > vertices.size()) {
        return false;
    }
    std::copy(newVertices, newVertices + vertexCount, vertices.begin() + offset);
    if (isInitialized) {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(Vertex), vertexCount * sizeof(Vertex), newVertices));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
    return true;
}

bool VertexBuffer::resize(std::size_t newSize) {
    vertices.resize(newSize);
    if (isInitialized && vertices.size() == gpuBuffferSize) {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    } else {
        recreateBuffer(vertices);
    }
    return true;
}

bool VertexBuffer::append(const Vertex& vertex) {
    vertices.push_back(vertex);
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
    GLenum usageGLenum;
    switch (usage) {
        case Usage::StaticDraw: usageGLenum = GL_STATIC_DRAW; break;
        case Usage::DynamicDraw: usageGLenum = GL_DYNAMIC_DRAW; break;
        case Usage::StreamDraw: usageGLenum = GL_STREAM_DRAW; break;
        default: throw std::invalid_argument("Invalid usage hint");
    }
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex), data.data(), usageGLenum));
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

Vertex& VertexBuffer::operator[](std::size_t index) {
    return vertices[index];
}

const Vertex& VertexBuffer::operator[](std::size_t index) const {
    return vertices[index];
}

}
