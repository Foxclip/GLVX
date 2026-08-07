#include "glvx/circle.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <corecrt_math_defines.h>
#include <cassert>
#include "glvx/glvx_common.h"
#include "glvx/texture.h"
#include "glvx/shader.h"
#include <iostream>

namespace glvx {

Circle::Circle(float radius, size_t num_segments) : Shape(PrimitiveType::TriangleFan, num_segments + 2) {
    assert(radius >= 0);
    assert(num_segments >= 3);
    m_radius = radius;
    m_num_segments = num_segments;
    createVertices();
}

Circle::~Circle() {
}

void Circle::setRadius(float radius) {
    assert(radius >= 0);
    m_radius = radius;
    createVertices();
}

void Circle::createVertices() {
    float theta = (float)(2.0 * M_PI / m_num_segments);

    // Add center vertex
    getVertex(0) = Vertex {
        Vector2f(m_radius, m_radius), // position
        Color::White, // color
        Vector2f(0.5f, 0.5f) // texCoords
    };

    for (size_t i = 0; i <= m_num_segments; i++) {
        float x = m_radius * cos(theta * i);
        float y = m_radius * sin(theta * i);
        // When i == m_num_segments, wrap around to 0 for closing the loop
        if (i == m_num_segments) {
            x = m_radius * cos(theta * 0);
            y = m_radius * sin(theta * 0);
        }
        float x_shifted = x + m_radius;
        float y_shifted = y + m_radius;
        float tex_x = (x / m_radius + 1.0f) / 2.0f;
        float tex_y = (y / m_radius + 1.0f) / 2.0f;
        getVertex(i + 1) = Vertex {
            Vector2f(x_shifted, y_shifted), // position
            Color::White, // color
            Vector2f(tex_x, tex_y) // tex_coords
        };
    }
}

}
