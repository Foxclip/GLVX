#include "glvx/transformable.h"
#include "glvx/transform.h"

namespace glvx {

float Transformable::getX() const {
    return m_position.x;
}

float Transformable::getY() const {
    return m_position.y;
}

const Vector2f& Transformable::getPosition() const {
    return m_position;
}

const Angle& Transformable::getRotation() const {
    return m_rotation;
}

const Vector2f& Transformable::getScale() const {
    return m_scale;
}

const Vector2f& Transformable::getOrigin() const {
    return m_origin;
}

void Transformable::setPosition(float x, float y) {
    m_position.x = x;
    m_position.y = y;
}

void Transformable::setPosition(const Vector2f& position) {
    m_position = position;
}

void Transformable::setRotation(const Angle& rotation) {
    m_rotation = rotation;
}

void Transformable::setScale(float x, float y) {
    m_scale.x = x;
    m_scale.y = y;
}

void Transformable::setScale(const Vector2f& scale) {
    m_scale = scale;
}

void Transformable::setOrigin(float x, float y) {
    m_origin.x = x;
    m_origin.y = y;
}

void Transformable::setOrigin(const Vector2f& origin) {
    m_origin = origin;
}

void Transformable::move(float dx, float dy) {
    m_position.x += dx;
    m_position.y += dy;
}

void Transformable::move(const Vector2f& offset) {
    m_position += offset;
}

Transform Transformable::getTransform() const {
    Transform result;
    result.translate(m_position.x, m_position.y);
    result.rotate(-m_rotation, Vector2f(0, 0));
    result.scale(m_scale.x, m_scale.y);
    result.translate(-m_origin.x, -m_origin.y);
    return result;
}

}
