#include "glvis/circle.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <corecrt_math_defines.h>
#include "glvis/glvis_common.h"
#include "glvis/texture.h"
#include "glvis/shader.h"
#include <iostream>

namespace glvis {

Circle::Circle(float radius, size_t numSegments) : Shape(PrimitiveType::TriangleFan, numSegments + 1) {
    this->radius = radius;
    this->numSegments = numSegments;

    float theta = (float)(2.0 * M_PI / numSegments);

    // Add center vertex
    getVertex(0) = Vertex {
        Vector2f(radius, radius), // position
        Color(255, 255, 255, 255), // color
        Vector2f(0.5f, 0.5f) // texCoords
    };

    for (size_t i = 0; i < numSegments; i++) {
        float x = radius * cos(theta * i);
        float y = radius * sin(theta * i);
        float x_shifted = x + radius;
        float y_shifted = y + radius;
        float texX = (x / radius + 1.0f) / 2.0f;
        float texY = (y / radius + 1.0f) / 2.0f;
        getVertex(i + 1) = Vertex {
            Vector2f(x_shifted, y_shifted), // position
            Color(255, 255, 255, 255), // color
            Vector2f(texX, texY) // texCoords
        };
    }
}

Circle::~Circle() {
}

}
