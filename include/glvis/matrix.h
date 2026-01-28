#pragma once

#include <array>
#include "glvis/vector.h"

namespace glvis {

class Matrix4 {
public:
    Matrix4();
    Matrix4(const std::array<float, 16>& data);
    Matrix4(const float* data);
    const float* getData() const;
    Matrix4 operator*(const Matrix4& other) const;

    static Matrix4 translation(const Vector3& v);

private:
    std::array<float, 16> data;
};

}
