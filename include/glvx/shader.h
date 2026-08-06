#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "glvx/matrix.h"
#include "glvx/vector.h"

namespace glvx {

enum class ShaderType {
    VERTEX,
    FRAGMENT
};

struct ShaderPart {
    std::string name;
    const char* source;
};

class Shader {
public:
    unsigned int m_id;
    bool m_use_ubo;

    Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, bool useUBO = false);
    Shader(const char* vertexSource, const char* fragmentSource, bool useUBO = false);
    Shader(
        const char* vertexSource,
        const char* fragmentTemplate,
        const std::vector<ShaderPart>& fragmentParts,
        bool useUBO = false
    );
    ~Shader();
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const Vector3& value) const;
    void setVec4(const std::string& name, const Vector4& value) const;
    void setMat4(const std::string& name, const Matrix4& value) const;
    bool uniformExists(const std::string& name) const;

private:
    int compileShader(ShaderType type, const std::filesystem::path& path);
    int compileShader(ShaderType type, const char* source);
    void linkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    std::string combineFragmentShader(
        const char* templateSource,
        const std::vector<ShaderPart>& parts
    );

};

}
