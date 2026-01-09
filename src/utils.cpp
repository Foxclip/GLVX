#include "glvis/utils.h"

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

    glm::vec3 to_glmVec3(const Vector2 & v) {
        return glm::vec3(v.x, v.y, 0.0f);
    }

}
