#include "glvis/utils.h"
#include <algorithm>
#include <cmath>
#include <memory>

namespace glvis {

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

glm::vec2 to_glmVec2(const Vector2 & v) {
    return glm::vec2(v.x, v.y);
}

Vector2 from_glmVec2(const glm::vec2& v) {
    return Vector2(v.x, v.y);
}

Vector3 toVector3(const Vector2 & v) {
    return Vector3(v.x, v.y, 0.0f);
}

glm::mat4 to_glmMat4(const Matrix4& m) {
    return glm::make_mat4(m.getData());
}

Matrix4 from_glmMat4(const glm::mat4& m) {
    return Matrix4(glm::value_ptr(m));
}

}
