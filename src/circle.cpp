#include "glvis/circle.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <corecrt_math_defines.h>
#include <cassert>
#include "glvis/glvis_common.h"
#include "glvis/texture.h"
#include "glvis/shader.h"
#include <iostream>

namespace glvis {

Circle::Circle(float radius, size_t numSegments) : Shape(PrimitiveType::TriangleFan, numSegments + 2) {
    assert(radius >= 0);
    assert(numSegments >= 3);
    this->radius = radius;
    this->numSegments = numSegments;
    createVertices();
}

Circle::~Circle() {
}

void Circle::setRadius(float radius) {
    assert(radius >= 0);
    this->radius = radius;
    createVertices();
}

void Circle::createVertices() {
    float theta = (float)(2.0 * M_PI / numSegments);

    // Add center vertex
    getVertex(0) = Vertex {
        Vector2f(radius, radius), // position
        Color::White, // color
        Vector2f(0.5f, 0.5f) // texCoords
    };

    for (size_t i = 0; i <= numSegments; i++) {
        float x = radius * cos(theta * i);
        float y = radius * sin(theta * i);
        // When i == numSegments, wrap around to 0 for closing the loop
        if (i == numSegments) {
            x = radius * cos(theta * 0);
            y = radius * sin(theta * 0);
        }
        float x_shifted = x + radius;
        float y_shifted = y + radius;
        float texX = (x / radius + 1.0f) / 2.0f;
        float texY = (y / radius + 1.0f) / 2.0f;
        getVertex(i + 1) = Vertex {
            Vector2f(x_shifted, y_shifted), // position
            Color::White, // color
            Vector2f(texX, texY) // texCoords
        };
    }
}

}
