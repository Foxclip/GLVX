#include "glvis/rectangle.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "glvis/glvis_common.h"
#include "glvis/texture.h"
#include "glvis/shader.h"

namespace glvis {

Rectangle::Rectangle(float width, float height) : Shape(PrimitiveType::Triangles, 6) {
    this->width = width;
    this->height = height;

    // Create vertices for rectangle (2 triangles, 6 vertices total)
    // First triangle (0, height), (0, 0), (width, height)
    getVertex(0).position = Vector2f(0.0f, height);
    getVertex(0).color = Color(255, 255, 255, 255);
    getVertex(0).texCoords = Vector2f(0.0f, 1.0f);

    getVertex(1).position = Vector2f(0.0f, 0.0f);
    getVertex(1).color = Color(255, 255, 255, 255);
    getVertex(1).texCoords = Vector2f(0.0f, 0.0f);

    getVertex(2).position = Vector2f(width, height);
    getVertex(2).color = Color(255, 255, 255, 255);
    getVertex(2).texCoords = Vector2f(1.0f, 1.0f);

    // Second triangle (width, height), (0, 0), (width, 0)
    getVertex(3).position = Vector2f(width, height);
    getVertex(3).color = Color(255, 255, 255, 255);
    getVertex(3).texCoords = Vector2f(1.0f, 1.0f);

    getVertex(4).position = Vector2f(0.0f, 0.0f);
    getVertex(4).color = Color(255, 255, 255, 255);
    getVertex(4).texCoords = Vector2f(0.0f, 0.0f);

    getVertex(5).position = Vector2f(width, 0.0f);
    getVertex(5).color = Color(255, 255, 255, 255);
    getVertex(5).texCoords = Vector2f(1.0f, 0.0f);
}

Rectangle::Rectangle(const Vector2f& size) : Rectangle(size.x, size.y) {}

Rectangle::~Rectangle() {
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

}
