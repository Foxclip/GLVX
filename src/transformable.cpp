#include "glvis/transformable.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glvis/utils.h"

namespace glvis {

float Transformable::getX() const {
    return position.x;
}

float Transformable::getY() const {
    return position.y;
}

const Vector2f& Transformable::getPosition() const {
    return position;
}

const Angle& Transformable::getRotation() const {
    return rotation;
}

const Vector2f& Transformable::getScale() const {
    return scale;
}

const Vector2f& Transformable::getOrigin() const {
    return origin;
}

void Transformable::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
}

void Transformable::setPosition(const Vector2f& position) {
    this->position = position;
}

void Transformable::setRotation(const Angle& rotation) {
    this->rotation = rotation;
}

void Transformable::setScale(float x, float y) {
    scale.x = x;
    scale.y = y;
}

void Transformable::setScale(const Vector2f& scale) {
    this->scale = scale;
}

void Transformable::setOrigin(float x, float y) {
    origin.x = x;
    origin.y = y;
}

void Transformable::setOrigin(const Vector2f& origin) {
    this->origin = origin;
}

void Transformable::move(float dx, float dy) {
    position.x += dx;
    position.y += dy;
}

void Transformable::move(const Vector2f& offset) {
    position += offset;
}

Matrix4 Transformable::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
    model = glm::rotate(model, rotation.asRadians(), glm::vec3(0.0f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
    model = glm::translate(model, glm::vec3(-origin.x, -origin.y, 0.0f));
    return from_glmMat4(model);
}

}
