#pragma once

#include <vector>
#include "glvis/glvis_common.h"
#include "glvis/vertex.h"
#include "glvis/matrix.h"

namespace glvis {

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
    bool create(std::size_t vertexCount);
    std::size_t getVertexCount() const;
    bool update(const std::vector<Vertex>& newVertices);
    bool update(const std::vector<Vertex>& newVertices, std::size_t vertexCount, unsigned int offset);
    PrimitiveType getPrimitiveType() const;
    void setPrimitiveType(PrimitiveType type);
    void setUsage(Usage usage);
    unsigned int getVAO() const;
    void render(const Matrix4& view, const Matrix4& projection) const;

private:
    bool isInitialized = false;
    size_t gpuBuffferSize = 0;
    size_t vertexCount = 0;
    PrimitiveType type = PrimitiveType::Triangles;
    Usage usage = Usage::StaticDraw;
    unsigned int VBO = 0;
    unsigned int VAO = 0;

    void recreateBuffer(std::size_t size);
    void updateBuffer(const void* data, unsigned int offset, std::size_t size);

};

}
