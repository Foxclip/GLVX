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
    Vector2 getBoundsMin() const;
    Vector2 getBoundsMax() const;
    Vertex& operator[](std::size_t index);
    const Vertex& operator[](unsigned int index) const;
    void clear();
    void resize(unsigned int newSize);
    void append(const Vertex& vertex);
    void setPrimitiveType(PrimitiveType type);
    void setShader(Shader* shader);
    void setTexture(AbstractTexture* texture);
    void syncBuffer();
    void render(const Matrix4& view, const Matrix4& projection) const override;
    const VertexBuffer& getVertexBuffer() const override;
    Matrix4 getModelMatrix() const override;

private:
    VertexBuffer vertexBuffer;
    Shader* shader = nullptr;
    AbstractTexture* texture = nullptr;
};

}
