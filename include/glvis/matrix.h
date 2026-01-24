#pragma once

#include <array>

namespace glvis {

class Matrix4 {
public:
    Matrix4();
    Matrix4(const std::array<float, 16>& data);
    Matrix4(const float* data);
    const float* getData() const;
    Matrix4 operator*(const Matrix4& other) const;

private:
    std::array<float, 16> data;
};

}
