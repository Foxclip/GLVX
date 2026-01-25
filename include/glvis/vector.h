#pragma once

namespace glvis {

class Vector2 {
public:
    Vector2();
    Vector2(float x, float y);
    float x = 0;
    float y = 0;

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

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

}
