#include "glvx/transformable.h"
#include "glvx/transform.h"

namespace glvx {

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

Transform Transformable::getTransform() const {
    Transform result;
    result.translate(position.x, position.y);
    result.rotate(-rotation, Vector2f(0, 0));
    result.scale(scale.x, scale.y);
    result.translate(-origin.x, -origin.y);
    return result;
}

}
