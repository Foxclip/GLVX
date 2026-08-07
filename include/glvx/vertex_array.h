#pragma once

#include <vector>
#include "glvx/vertex.h"
#include "glvx/vertex_buffer.h"
#include "glvx/drawable.h"

namespace glvx {

class Shader;
class AbstractTexture;

class VertexArray : public Drawable {
public:
    VertexArray();
    VertexArray(PrimitiveType type, std::size_t vertexCount = 0);
    std::size_t getVertexCount() const;
    PrimitiveType getPrimitiveType() const;
    Vector2f getBoundsMin() const;
    Vector2f getBoundsMax() const;
    Vertex& operator[](std::size_t index);
    const Vertex& operator[](std::size_t index) const;
    Vertex& getVertex(std::size_t index);
    const Vertex& getVertex(std::size_t index) const;
    void clear();
    void resize(unsigned int newSize);
    void append(const Vertex& vertex);
    void setPrimitiveType(PrimitiveType type);
    void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const override;
    const VertexBuffer& getVertexBuffer() const override;
    Transform getTransform() const override;

private:
    std::vector<Vertex> m_vertices;
    mutable VertexBuffer m_vertex_buffer;
};

}
