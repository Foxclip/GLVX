#include "glvx/matrix.h"
#include <cstring>
#include <cassert>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "glvx/utils.h"

namespace glvx {

Matrix4::Matrix4() : data(1.0f) {}

Matrix4::Matrix4(const std::array<float, 16>& data) : data(
    data[0], data[1], data[2], data[3],
    data[4], data[5], data[6], data[7],
    data[8], data[9], data[10], data[11],
    data[12], data[13], data[14], data[15]
) {}

Matrix4::Matrix4(const float* data) : data(
    data[0], data[1], data[2], data[3],
    data[4], data[5], data[6], data[7],
    data[8], data[9], data[10], data[11],
    data[12], data[13], data[14], data[15]
) {}

Matrix4::Matrix4(const glm::mat4& data) : data(data) {}

const float* Matrix4::getData() const {
    return &data[0][0];
}

float Matrix4::get(int row, int col) const {
    return data[row][col];
}

float& Matrix4::get(int row, int col) {
    return data[row][col];
}

std::string Matrix4::toString() const {
    constexpr std::string_view floatFormatStr = "{:.3f}";
    auto count_digits = [](float val) {
		int result = 1;
		float absval = abs(val);
		if (absval > 1) {
			result += static_cast<int>(log10(absval));
		}
		if (std::signbit(val)) {
			result++;
		}
		return result;
	};
    int column_shifts[4] = {0, 0, 0, 0};
    for (int x = 0; x < 4; x++) {
        int max_shift = 0;
        for (int y = 0; y < 4; y++) {
            float val = get(x, y);
            int digit_count = count_digits(val);
            if (digit_count > max_shift) {
                max_shift = digit_count;
            }
        }
        column_shifts[x] = max_shift;
    }
    std::string str = "";
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            float val = get(x, y);
            int digit_count = count_digits(val);
            int spaces = std::max(0, column_shifts[x] - digit_count);
            for (int i = 0; i < spaces; i++) {
                str += " ";
            }
            str += std::format(floatFormatStr, get(x, y));
            if (x != 4 - 1) {
                str += " ";
            }
        }
        if (y < 4 - 1) {
            str += "\n";
        }
    }
    return str;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    return Matrix4(data * other.data);
}

bool Matrix4::operator==(const Matrix4& other) const {
    return data == other.data;
}

Matrix4 Matrix4::translate(const Matrix4& matrix, const Vector3& v) {
    return Matrix4(glm::translate(matrix.data, glm::vec3(v.x, v.y, v.z)));
}

Matrix4 Matrix4::rotate(const Matrix4& matrix, float angle, const Vector3& axis) {
    return Matrix4(glm::rotate(matrix.data, angle, glm::vec3(axis.x, axis.y, axis.z)));
}

Matrix4 Matrix4::scale(const Matrix4& matrix, const Vector3& v) {
    return Matrix4(glm::scale(matrix.data, glm::vec3(v.x, v.y, v.z)));
}

Matrix4 Matrix4::ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
    return Matrix4(glm::ortho(left, right, bottom, top, zNear, zFar));
}

}
