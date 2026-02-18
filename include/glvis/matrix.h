#pragma once

#include <array>
#include <string>
#include <glm/mat4x4.hpp>
#include "glvis/vector.h"

namespace glvis {

class Matrix4 {
public:
    Matrix4();
    Matrix4(const std::array<float, 16>& data);
    Matrix4(const float* data);
    explicit Matrix4(const glm::mat4& data);
    const float* getData() const;
    float get(int row, int col) const;
    float& get(int row, int col);
    std::string toString() const;
    Matrix4 operator*(const Matrix4& other) const;
    static Matrix4 translate(const Matrix4& matrix, const Vector3& v);
    static Matrix4 rotate(const Matrix4& matrix, float angle, const Vector3& axis);
    static Matrix4 scale(const Matrix4& matrix, const Vector3& v);
    static Matrix4 ortho(float left, float right, float bottom, float top, float zNear, float zFar);

private:
    glm::mat4 data;
};

}
