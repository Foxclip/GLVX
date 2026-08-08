#include "glvx/utils.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>

namespace glvx {

std::string file_to_str(const std::filesystem::path& path) {
    try {
        if (!std::filesystem::exists(path)) {
            throw std::format("File not found: {}", path.string());
        }
        std::ifstream t(path);
        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    } catch (std::exception& e) {
        throw std::runtime_error(__FUNCTION__": " + std::string(e.what()));
    }
}

void str_to_file(const std::string& str, const std::filesystem::path& path) {
    std::ofstream out_stream(path);
    if (out_stream.is_open()) {
        out_stream << str;
    } else {
        std::string p(path.string());
        p.resize(FILENAME_MAX);
        throw std::runtime_error("File write error: " + p);
    }
}

glm::vec2 to_glmVec2(const Vector2f & v) {
    return glm::vec2(v.x, v.y);
}

Vector2f from_glmVec2(const glm::vec2& v) {
    return Vector2f(v.x, v.y);
}

Vector3 toVector3(const Vector2f & v) {
    return Vector3(v.x, v.y, 0.0f);
}

glm::mat4 to_glmMat4(const Matrix4& m) {
    return glm::make_mat4(m.getData());
}

Matrix4 from_glmMat4(const glm::mat4& m) {
    return Matrix4(glm::value_ptr(m));
}

void blit_bitmap(
    const unsigned char* src, int srcPitch,
    unsigned char* dst, int dstPitch,
    int x, int y, int width, int height
) {
    for (int row = 0; row < height; row++) {
        const unsigned char* srcRow = src + row * srcPitch;
        unsigned char* dstRow = dst + (y + row) * dstPitch + x;
        std::memcpy(dstRow, srcRow, width);
    }
}

void blit_bitmap_subpixel(
    const unsigned char* src, int srcPitch,
    unsigned char* dst, int dstPitch,
    int x, int y, int width, int height
) {
    for (int row = 0; row < height; row++) {
        const unsigned char* srcRow = src + row * srcPitch;
        unsigned char* dstRow = dst + (y + row) * dstPitch + x * 3;
        std::memcpy(dstRow, srcRow, width);
    }
}

}
