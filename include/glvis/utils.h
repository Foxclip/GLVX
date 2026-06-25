#pragma once

#include <fstream>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "glvis/vector.h"
#include "glvis/matrix.h"

namespace glvis {

std::string file_to_str(const std::filesystem::path& path);
void str_to_file(const std::string& str, const std::filesystem::path& path);
glm::vec2 to_glmVec2(const Vector2f& v);
Vector2f from_glmVec2(const glm::vec2& v);
Vector3 toVector3(const Vector2f & v);
glm::mat4 to_glmMat4(const Matrix4& m);
Matrix4 from_glmMat4(const glm::mat4& m);

void blit_bitmap(
    const unsigned char* src, int srcPitch,
    unsigned char* dst, int dstPitch,
    int x, int y, int width, int height
);

}
