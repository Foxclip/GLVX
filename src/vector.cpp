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

Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2i::Vector2i() {
    this->x = 0;
    this->y = 0;
}

Vector2i::Vector2i(int x, int y) {
    this->x = x;
    this->y = y;
}

Vector2i Vector2i::operator+(const Vector2i& other) const {
    return Vector2i(x + other.x, y + other.y);
}

Vector2i Vector2i::operator-(const Vector2i& other) const {
    return Vector2i(x - other.x, y - other.y);
}

Vector2i Vector2i::operator*(int scalar) const {
    return Vector2i(x * scalar, y * scalar);
}

Vector2i Vector2i::operator/(int scalar) const {
    return Vector2i(x / scalar, y / scalar);
}

Vector3::Vector3() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Vector3::Vector3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector4::Vector4() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 0;
}

Vector4::Vector4(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Vector4 Vector4::operator+(const Vector4& other) const {
    return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4 Vector4::operator-(const Vector4& other) const {
    return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4 Vector4::operator*(float scalar) const {
    return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4 Vector4::operator/(float scalar) const {
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

}
