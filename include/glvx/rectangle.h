#pragma once

#include "glvx/vector.h"
#include "glvx/shape.h"

namespace glvx {

class AbstractTexture;

class Rectangle : public Shape {
public:
    Rectangle(float width, float height);
    Rectangle(const Vector2f& size);
    ~Rectangle();
    float getWidth() const;
    float getHeight() const;
    Vector2f getSize() const;
    void setSize(float width, float height);
    void setSize(const Vector2f& size);

private:
    float m_width = 0.0f;
    float m_height = 0.0f;

};

}
