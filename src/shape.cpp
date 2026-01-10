#include "glvis/shape.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glvis {

float Shape::getX() const {
    return position.x;
}

float Shape::getY() const {
    return position.y;
}

const Vector2& Shape::getPosition() const {
    return position;
}

const Angle& Shape::getRotation() const {
    return rotation;
}

const Vector2& Shape::getScale() const {
    return scale;
}

const Vector2& Shape::getOrigin() const {
    return origin;
}

void Shape::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
}

void Shape::setPosition(const Vector2& position) {
    this->position = position;
}

void Shape::setRotation(const Angle& rotation) {
    this->rotation = rotation;
}

void Shape::setScale(float x, float y) {
    scale.x = x;
    scale.y = y;
}

void Shape::setScale(const Vector2& scale) {
    this->scale = scale;
}

void Shape::setOrigin(float x, float y) {
    origin.x = x;
    origin.y = y;
}

void Shape::setOrigin(const Vector2& origin) {
    this->origin = origin;
}

void Shape::setShader(Shader* shader) {
    this->shader = shader;
}

glm::mat4 Shape::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
    model = glm::rotate(model, rotation.asRadians(), glm::vec3(0.0f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
    model = glm::translate(model, glm::vec3(-origin.x, -origin.y, 0.0f));
    return model;
}

}
