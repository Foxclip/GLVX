#pragma once

#include "glvx/shape.h"

namespace glvx {

class Circle : public Shape {
public:
    Circle(float radius = 0, size_t numSegments = 30);
    ~Circle();

    void setRadius(float radius);

private:
    void createVertices();

    float m_radius = 0.0f;
    size_t m_num_segments = 0;

};

}
