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

glm::vec3 to_glmVec3(const Vector2 & v) {
    return glm::vec3(v.x, v.y, 0.0f);
}

std::unique_ptr<unsigned char[]> bilinearInterpolate(
    const unsigned char* oldData, int oldWidth, int oldHeight, int newWidth, int newHeight
) {
    std::unique_ptr<unsigned char[]> newData(new unsigned char[newWidth * newHeight * 4]);
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            float srcX = (newWidth == 1) ? 0.0f : (x * (oldWidth - 1.0f) / (newWidth - 1.0f));
            float srcY = (newHeight == 1) ? 0.0f : (y * (oldHeight - 1.0f) / (newHeight - 1.0f));
            int x0 = static_cast<int>(floor(srcX));
            int y0 = static_cast<int>(floor(srcY));
            int x1 = std::min(x0 + 1, oldWidth - 1);
            int y1 = std::min(y0 + 1, oldHeight - 1);
            float factorX = srcX - x0;
            float factorY = srcY - y0;
            for (int channel = 0; channel < 4; channel++) {
                float v00 = oldData[(y0 * oldWidth + x0) * 4 + channel];
                float v01 = oldData[(y0 * oldWidth + x1) * 4 + channel];
                float v10 = oldData[(y1 * oldWidth + x0) * 4 + channel];
                float v11 = oldData[(y1 * oldWidth + x1) * 4 + channel];
                float v0 = v00 * (1.0f - factorX) + v01 * factorX;
                float v1 = v10 * (1.0f - factorX) + v11 * factorX;
                newData[(y * newWidth + x) * 4 + channel] = static_cast<unsigned char>(
                    v0 * (1.0f - factorY) + v1 * factorY
                );
            }
        }
    }
    return newData;
}

}
