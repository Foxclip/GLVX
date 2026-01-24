#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include "glvis/matrix.h"

namespace glvis {

enum class ShaderType {
    VERTEX,
    FRAGMENT
};

class Shader {
public:
    unsigned int ID;

    Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();
    void use();
    void setBool(const std::string& name, bool value) const;  
    void setInt(const std::string& name, int value) const;   
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const Matrix4& value) const;

private:
    int compileShader(ShaderType type, const std::filesystem::path& path);
    int compileShader(ShaderType type, const char* source);

};

}
