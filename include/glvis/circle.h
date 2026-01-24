#pragma once

#include "glvis/shape.h"
#include <glm/glm.hpp>
#include "glvis/texture.h"
#include "glvis/vertex_buffer.h"

namespace glvis {

class Circle : public Shape {
public:
    Circle(float radius = 0, size_t numSegments = 30);
    ~Circle();
    void setTexture(AbstractTexture* texture);
    void render(const Matrix4& view, const Matrix4& projection) const override;
    const VertexBuffer& getVertexBuffer() const override;

private:
    AbstractTexture* texture = nullptr;
    float radius = 0.0f;
    size_t numSegments = 0;
    VertexBuffer vertexBuffer;
    
};

}
