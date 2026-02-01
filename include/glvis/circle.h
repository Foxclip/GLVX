#pragma once

#include "glvis/shape.h"
#include "glvis/texture.h"
#include "glvis/vertex_buffer.h"

namespace glvis {

class Circle : public Shape {
public:
    Circle(float radius = 0, size_t numSegments = 30);
    ~Circle();
    const VertexBuffer& getVertexBuffer() const override;

private:
    float radius = 0.0f;
    size_t numSegments = 0;
    VertexBuffer vertexBuffer;

};

}
