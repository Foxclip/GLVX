#include "glvis/matrix.h"
#include <cstring>
#include <cassert>
#include <string>
#include "glvis/utils.h"

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
    assert(data);
    std::memcpy(this->data.data(), data, sizeof(float) * 16);
}

const float* Matrix4::getData() const {
    return data.data();
}

float Matrix4::get(int row, int col) const {
    return data[row * 4 + col];
}

std::string Matrix4::toString() const {
    constexpr std::string_view floatFormatStr = "{:.3f}";
    auto count_digits = [](float val) {
		int result = 1;
		float absval = abs(val);
		if (absval > 1) {
			result += (int)log10(absval);
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
