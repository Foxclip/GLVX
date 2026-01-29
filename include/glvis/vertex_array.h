#pragma once

#include <vector>
#include "glvis/vertex.h"
#include "glvis/vertex_buffer.h"
#include "glvis/drawable.h"

namespace glvis {

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
    void clear();
    void resize(unsigned int newSize);
    void append(const Vertex& vertex);
    void setPrimitiveType(PrimitiveType type);
    void setShader(Shader* shader);
    void setTexture(AbstractTexture* texture);
    void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const override;
    const VertexBuffer& getVertexBuffer() const override;
    Matrix4 getModelMatrix() const override;

private:
    std::vector<Vertex> vertices;
    mutable VertexBuffer vertexBuffer;
    Shader* shader = nullptr;
    AbstractTexture* texture = nullptr;
};

}
