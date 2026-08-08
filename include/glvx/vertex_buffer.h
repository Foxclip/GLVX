#pragma once

#include <vector>
#include "glvx/glvx_common.h"
#include "glvx/vertex.h"
#include "glvx/matrix.h"

namespace glvx {

enum class Usage {
    StaticDraw,
    DynamicDraw,
    StreamDraw
};

class VertexBuffer {
public:
    VertexBuffer() = default;
    VertexBuffer(PrimitiveType type);
    VertexBuffer(Usage usage);
    VertexBuffer(PrimitiveType type, Usage usage);
    ~VertexBuffer();
    bool create(std::size_t vertex_count);
    std::size_t getVertexCount() const;
    bool update(const std::vector<Vertex>& new_vertices);
    bool update(const std::vector<Vertex>& new_vertices, std::size_t vertex_count, unsigned int offset);
    PrimitiveType getPrimitiveType() const;
    void setPrimitiveType(PrimitiveType type);
    void setUsage(Usage usage);
    unsigned int getVAO() const;
    void render() const;

private:
    bool m_is_initialized = false;
    size_t m_gpu_buffer_size = 0;
    size_t m_vertex_count = 0;
    PrimitiveType m_type = PrimitiveType::Triangles;
    Usage m_usage = Usage::StaticDraw;
    unsigned int m_vbo = 0;
    unsigned int m_vao = 0;

    void ensureInitialized(std::size_t size);
    void recreateBuffer(std::size_t size);
    void updateBuffer(const void* data, unsigned int offset, std::size_t size);

};

}
