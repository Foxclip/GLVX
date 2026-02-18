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
    std::string toString() const;
    Matrix4 operator*(const Matrix4& other) const;
    static Matrix4 translate(const Matrix4& matrix, const Vector3& v);

    // Implicit conversion to glm::mat4 for interoperability
    operator glm::mat4() const;

private:
    glm::mat4 data;
};

}
