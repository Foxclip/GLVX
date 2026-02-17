#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/vector_int2.hpp>

namespace glvis {

class Vector2f;

class Vector2i : public glm::ivec2 {
public:
    using glm::ivec2::ivec2;
    Vector2i();
    Vector2i(int x, int y);
    Vector2i(const Vector2f& v);

    Vector2i operator+(const Vector2i& other) const;
    Vector2i operator-(const Vector2i& other) const;
    Vector2i operator*(int scalar) const;
    Vector2i operator/(int scalar) const;
    Vector2i operator-() const;
};

class Vector2f : public glm::vec2 {
public:
    using glm::vec2::vec2;
    Vector2f();
    Vector2f(float x, float y);
    explicit Vector2f(const Vector2i& v);

    Vector2f operator+(const Vector2f& other) const;
    Vector2f operator-(const Vector2f& other) const;
    Vector2f operator*(float scalar) const;
    Vector2f operator/(float scalar) const;
    Vector2f& operator+=(const Vector2f& other);
    Vector2f operator-() const;
};

class Vector3 : public glm::vec3 {
public:
    using glm::vec3::vec3;
    Vector3();
    Vector3(float x, float y, float z);

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3 operator-() const;
};

class Vector4 : public glm::vec4 {
public:
    using glm::vec4::vec4;
    Vector4();
    Vector4(float x, float y, float z, float w);

    Vector4 operator+(const Vector4& other) const;
    Vector4 operator-(const Vector4& other) const;
    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;
    Vector4 operator-() const;
};

}
