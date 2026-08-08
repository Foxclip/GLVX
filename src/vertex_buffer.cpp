#include "glvx/vertex_buffer.h"
#include <stdexcept>
#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvx/vertex.h"

namespace glvx {

VertexBuffer::VertexBuffer(PrimitiveType type) : VertexBuffer(type, Usage::StaticDraw) { }

VertexBuffer::VertexBuffer(Usage usage) : VertexBuffer(PrimitiveType::Triangles, usage) { }

VertexBuffer::VertexBuffer(PrimitiveType type, Usage usage): m_vbo(0), m_vao(0) {
    m_type = type;
    m_usage = usage;
}

VertexBuffer::~VertexBuffer() {
    if (!has_active_gl_context()) {
        return;
    }
    if (m_vao != 0) {
        GL_CALL(glDeleteVertexArrays(1, &m_vao));
        m_vao = 0;
    }
    if (m_vbo != 0) {
        GL_CALL(glDeleteBuffers(1, &m_vbo));
        m_vbo = 0;
    }
}

bool VertexBuffer::create(std::size_t vertex_count) {
    m_vertex_count = vertex_count;
    if (m_vao == 0) {
        return true;
    }
    recreateBuffer(vertex_count);
    return true;
}

std::size_t VertexBuffer::getVertexCount() const {
    return m_vertex_count;
}

void VertexBuffer::ensureInitialized(std::size_t size) {
    if (size == 0) {
        return;
    }
    if (m_vao == 0) {
        GL_CALL(glGenVertexArrays(1, &m_vao));
    }
    recreateBuffer(size);
}

bool VertexBuffer::update(const std::vector<Vertex>& new_vertices) {
    ensureInitialized(new_vertices.size());
    if (new_vertices.size() != m_vertex_count) {
        m_vertex_count = new_vertices.size();
        recreateBuffer(m_vertex_count);
    }
    updateBuffer(new_vertices.data(), 0, new_vertices.size() * sizeof(Vertex));
    return true;
}

bool VertexBuffer::update(const std::vector<Vertex>& new_vertices, std::size_t vertex_count, unsigned int offset) {
    if (offset + vertex_count > m_vertex_count) {
        return false;
    }
    updateBuffer(new_vertices.data(), offset * sizeof(Vertex), vertex_count * sizeof(Vertex));
    return true;
}

void VertexBuffer::updateBuffer(const void* data, unsigned int offset, std::size_t size) {
    if (m_vbo == 0) {
        return;
    }
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::recreateBuffer(std::size_t size) {
    if (m_vbo != 0) {
        GL_CALL(glDeleteBuffers(1, &m_vbo));
        m_vbo = 0;
    }
    GL_CALL(glGenBuffers(1, &m_vbo));
    GL_CALL(glBindVertexArray(m_vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GLenum usage_GLenum;
    switch (m_usage) {
        case Usage::StaticDraw: usage_GLenum = GL_STATIC_DRAW; break;
        case Usage::DynamicDraw: usage_GLenum = GL_DYNAMIC_DRAW; break;
        case Usage::StreamDraw: usage_GLenum = GL_STREAM_DRAW; break;
        default: throw std::invalid_argument("Invalid usage hint");
    }
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), nullptr, usage_GLenum));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0)));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color))));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tex_coords))));
    GL_CALL(glEnableVertexAttribArray(2));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindVertexArray(0));
    m_is_initialized = true;
    m_gpu_buffer_size = size;
}

PrimitiveType VertexBuffer::getPrimitiveType() const {
    return m_type;
}

void VertexBuffer::setPrimitiveType(PrimitiveType type) {
    m_type = type;
}

void VertexBuffer::setUsage(Usage usage) {
    m_usage = usage;
}

unsigned int VertexBuffer::getVAO() const {
    return m_vao;
}

void VertexBuffer::render() const {
    if (m_vertex_count == 0) {
        return;
    }
    if (m_vao == 0) {
        return;
    }
    GL_CALL(glBindVertexArray(m_vao));
    GL_CALL(glDrawArrays(static_cast<GLenum>(m_type), 0, static_cast<GLsizei>(getVertexCount())));
}

}
