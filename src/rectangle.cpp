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
    (*this)[0].position = Vector2f(0.0f, height);
    (*this)[0].color = Color(255, 255, 255, 255);
    (*this)[0].texCoords = Vector2f(0.0f, 1.0f);

    (*this)[1].position = Vector2f(0.0f, 0.0f);
    (*this)[1].color = Color(255, 255, 255, 255);
    (*this)[1].texCoords = Vector2f(0.0f, 0.0f);

    (*this)[2].position = Vector2f(width, height);
    (*this)[2].color = Color(255, 255, 255, 255);
    (*this)[2].texCoords = Vector2f(1.0f, 1.0f);

    // Second triangle (width, height), (0, 0), (width, 0)
    (*this)[3].position = Vector2f(width, height);
    (*this)[3].color = Color(255, 255, 255, 255);
    (*this)[3].texCoords = Vector2f(1.0f, 1.0f);

    (*this)[4].position = Vector2f(0.0f, 0.0f);
    (*this)[4].color = Color(255, 255, 255, 255);
    (*this)[4].texCoords = Vector2f(0.0f, 0.0f);

    (*this)[5].position = Vector2f(width, 0.0f);
    (*this)[5].color = Color(255, 255, 255, 255);
    (*this)[5].texCoords = Vector2f(1.0f, 0.0f);
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
