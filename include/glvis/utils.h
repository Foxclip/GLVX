#pragma once

#include <fstream>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glvis/vector.h"

namespace glvis {

std::string file_to_str(const std::filesystem::path& path);
glm::vec2 to_glmVec2(const Vector2& v);
glm::vec3 to_glmVec3(const Vector2 & v);

}
