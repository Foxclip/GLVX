#include "glvis/vector.h"

namespace glvis {

Vector2::Vector2() {
    this->x = 0;
    this->y = 0;
}

Vector2::Vector2(float x, float y) {
    this->x = x;
    this->y = y;
}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

}
