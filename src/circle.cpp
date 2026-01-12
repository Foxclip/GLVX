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

Circle::Circle(float radius, size_t numSegments) {
    this->radius = radius;
    this->numSegments = numSegments;
    this->shader = common::defaultShader;

    float theta = (float)(2.0 * M_PI / numSegments);
    std::vector<Vertex> vertices;
    // Add center vertex
    vertices.push_back(Vertex {
        Vector2(radius, radius), // position
        Color(1.0f, 1.0f, 1.0f, 1.0f), // color
        Vector2(0.5f, 0.5f) // texCoords
    });
    for (size_t i = 0; i <= numSegments; i++) {
        float x = radius * cos(theta * i);
        float y = radius * sin(theta * i);
        float x_shifted = x + radius;
        float y_shifted = y + radius;
        float texX = (x / radius + 1.0f) / 2.0f;
        float texY = (y / radius + 1.0f) / 2.0f;
        vertices.push_back(Vertex {
            Vector2(x_shifted, y_shifted), // position
            Color(1.0f, 1.0f, 1.0f, 1.0f), // color
            Vector2(texX, texY) // texCoords
        });
    }

    // Initialize vertex buffer with all vertices
    vertexBuffer.create(vertices.size());
    vertexBuffer.update(vertices);
    vertexBuffer.setPrimitiveType(PrimitiveType::TriangleFan);
}

Circle::~Circle() {
    // VertexBuffer destructor will handle cleanup
}

void Circle::setTexture(AbstractTexture* texture) {
    this->texture = texture;
}

void Circle::render(const glm::mat4& view, const glm::mat4& projection) const {
    START_TRY
        if (shader == nullptr) throw std::runtime_error("Shader not set");
        auto modelMatrix = getModelMatrix();
        shader->use();
        shader->setMat4("model", modelMatrix);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        renderBase(shader, texture, view, projection);
    END_TRY
}

const VertexBuffer& Circle::getVertexBuffer() const {
    return vertexBuffer;
}

}
