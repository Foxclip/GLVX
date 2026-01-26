#pragma once

namespace glvis {

class Vector2f {
public:
    Vector2f();
    Vector2f(float x, float y);
    float x = 0;
    float y = 0;

    Vector2f operator+(const Vector2f& other) const;
    Vector2f operator-(const Vector2f& other) const;
    Vector2f operator*(float scalar) const;
    Vector2f operator/(float scalar) const;
    Vector2f& operator+=(const Vector2f& other);

private:
};

class Vector2i {
public:
    Vector2i();
    Vector2i(int x, int y);
    int x = 0;
    int y = 0;

    Vector2i operator+(const Vector2i& other) const;
    Vector2i operator-(const Vector2i& other) const;
    Vector2i operator*(int scalar) const;
    Vector2i operator/(int scalar) const;

private:
};

class Vector3 {
public:
    Vector3();
    Vector3(float x, float y, float z);
    float x = 0;
    float y = 0;
    float z = 0;

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

private:
};

class Vector4 {
public:
    Vector4();
    Vector4(float x, float y, float z, float w);
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    Vector4 operator+(const Vector4& other) const;
    Vector4 operator-(const Vector4& other) const;
    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;

private:
};

}
