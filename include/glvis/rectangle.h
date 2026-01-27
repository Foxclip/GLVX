#pragma once

#include "glvis/vector.h"
#include "glvis/shape.h"
#include "glvis/vertex_buffer.h"

namespace glvis {

class AbstractTexture;

class Rectangle : public Shape {
public:
    Rectangle(float width, float height);
    Rectangle(const Vector2f& size);
    ~Rectangle();
    float getWidth() const;
    float getHeight() const;
    Vector2f getSize() const;
    void setTexture(AbstractTexture* texture);
    void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const override;
    const VertexBuffer& getVertexBuffer() const override;

private:
    AbstractTexture* texture = nullptr;
    float width = 0.0f;
    float height = 0.0f;
    VertexBuffer vertexBuffer;

};

}
