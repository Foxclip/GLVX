#include "glvis/matrix.h"
#include <cstring>

namespace glvis {

Matrix4::Matrix4() {
    // Identity matrix
    data = {1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
}

Matrix4::Matrix4(const std::array<float, 16>& data) : data(data) {}

Matrix4::Matrix4(const float* data) {
    std::memcpy(this->data.data(), data, sizeof(float) * 16);
}

const float* Matrix4::getData() const {
    return data.data();
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    // Simple matrix multiplication (column-major)
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.data[i * 4 + j] += data[k * 4 + j] * other.data[i * 4 + k];
            }
        }
    }
    return result;
}

Matrix4 Matrix4::translate(const Matrix4& matrix, const Vector3& v) {
    Matrix4 result = matrix;
    // Add translation to the 4th column (positions 12, 13, 14 in column-major order)
    result.data[12] += v.x;
    result.data[13] += v.y;
    result.data[14] += v.z;
    return result;
}

}
