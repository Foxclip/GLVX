#include "glvis/shape.h"

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
        this->position.x = x;
        this->position.y = y;
    }

    void Shape::setPosition(const Vector2& position) {
        this->position = position;
    }

    void Shape::setRotation(const Angle& rotation) {
        this->rotation = rotation;
    }

    void Shape::setScale(float x, float y) {
        this->scale.x = x;
        this->scale.y = y;
    }

    void Shape::setScale(const Vector2& scale) {
        this->scale = scale;
    }

    void Shape::setOrigin(float x, float y) {
        this->origin.x = x;
        this->origin.y = y;
    }

    void Shape::setOrigin(const Vector2& origin) {
        this->origin = origin;
    }

    void Shape::setShader(Shader* shader) {
        this->shader = shader;
    }

}
