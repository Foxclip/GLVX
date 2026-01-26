#include "glvis/rectangle.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "glvis/glvis_common.h"
#include "glvis/texture.h"
#include "glvis/shader.h"

namespace glvis {

Rectangle::Rectangle(float width, float height) {
    this->width = width;
    this->height = height;
    this->shader = common::defaultShader;

    // Create vertices for rectangle (2 triangles, 6 vertices total)
    std::vector<Vertex> vertices(6);
    // First triangle (0, height), (0, 0), (width, height)
    vertices[0].position = Vector2f(0.0f, height);
    vertices[0].color = Color(255, 255, 255, 255);
    vertices[0].texCoords = Vector2f(0.0f, 1.0f);
    
    vertices[1].position = Vector2f(0.0f, 0.0f);
    vertices[1].color = Color(255, 255, 255, 255);
    vertices[1].texCoords = Vector2f(0.0f, 0.0f);
    
    vertices[2].position = Vector2f(width, height);
    vertices[2].color = Color(255, 255, 255, 255);
    vertices[2].texCoords = Vector2f(1.0f, 1.0f);
    
    // Second triangle (width, height), (0, 0), (width, 0)
    vertices[3].position = Vector2f(width, height);
    vertices[3].color = Color(255, 255, 255, 255);
    vertices[3].texCoords = Vector2f(1.0f, 1.0f);
    
    vertices[4].position = Vector2f(0.0f, 0.0f);
    vertices[4].color = Color(255, 255, 255, 255);
    vertices[4].texCoords = Vector2f(0.0f, 0.0f);
    
    vertices[5].position = Vector2f(width, 0.0f);
    vertices[5].color = Color(255, 255, 255, 255);
    vertices[5].texCoords = Vector2f(1.0f, 0.0f);

    // Initialize vertex buffer with 6 vertices
    vertexBuffer.create(6);
    vertexBuffer.update(vertices);
    vertexBuffer.setPrimitiveType(PrimitiveType::Triangles);
}

Rectangle::Rectangle(const Vector2f& size) : Rectangle(size.x, size.y) {}

Rectangle::~Rectangle() {
    // VertexBuffer destructor will handle cleanup
}

float Rectangle::getWidth() const {
    return width;
}

float Rectangle::getHeight() const {
    return height;
}

Vector2f Rectangle::getSize() const {
    return Vector2f(width, height);
}

void Rectangle::setTexture(AbstractTexture* texture) {
    this->texture = texture;
}

void Rectangle::render(const Matrix4& view, const Matrix4& projection) const {
    START_TRY
    if (shader == nullptr) throw std::runtime_error("Shader not set");
    auto modelMatrix = getModelMatrix();
    shader->use();
    shader->setMat4("model", modelMatrix);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    renderBase(shader, texture, view, projection);
    END_TRY
}

const VertexBuffer& Rectangle::getVertexBuffer() const {
    return vertexBuffer;
}

}
