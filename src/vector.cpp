#include "glvis/vector.h"

namespace glvis {

// Vector2f
Vector2f::Vector2f() : glm::vec2(0.0f, 0.0f) {}

Vector2f::Vector2f(float x, float y) : glm::vec2(x, y) {}

Vector2f::Vector2f(const Vector2i& v) : glm::vec2(static_cast<float>(v.x), static_cast<float>(v.y)) {}

Vector2f Vector2f::operator+(const Vector2f& other) const {
    return Vector2f(x + other.x, y + other.y);
}

Vector2f Vector2f::operator-(const Vector2f& other) const {
    return Vector2f(x - other.x, y - other.y);
}

Vector2f Vector2f::operator*(float scalar) const {
    return Vector2f(x * scalar, y * scalar);
}

Vector2f Vector2f::operator/(float scalar) const {
    return Vector2f(x / scalar, y / scalar);
}

Vector2f& Vector2f::operator+=(const Vector2f& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2f Vector2f::operator-() const {
    return Vector2f(-x, -y);
}

// Vector2i
Vector2i::Vector2i() : glm::ivec2(0, 0) {}

Vector2i::Vector2i(int x, int y) : glm::ivec2(x, y) {}

Vector2i::Vector2i(const Vector2f& v) : glm::ivec2(static_cast<int>(v.x), static_cast<int>(v.y)) {}

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

Vector2i Vector2i::operator-() const {
    return Vector2i(-x, -y);
}

// Vector3
Vector3::Vector3() : glm::vec3(0.0f, 0.0f, 0.0f) {}

Vector3::Vector3(float x, float y, float z) : glm::vec3(x, y, z) {}

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

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

// Vector4
Vector4::Vector4() : glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) {}

Vector4::Vector4(float x, float y, float z, float w) : glm::vec4(x, y, z, w) {}

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

Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

}
