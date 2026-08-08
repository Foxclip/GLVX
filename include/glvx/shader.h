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
    Shader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path, bool use_ubo = false);
    Shader(const char* vertex_source, const char* fragment_source, bool use_ubo = false);
    Shader(
        const char* vertex_source,
        const char* fragment_template,
        const std::vector<ShaderPart>& fragment_parts,
        bool use_ubo = false
    );
    ~Shader();
    unsigned int getId() const;
    bool isUsingUBO() const;
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const Vector3& value) const;
    void setVec4(const std::string& name, const Vector4& value) const;
    void setMat4(const std::string& name, const Matrix4& value) const;
    bool uniformExists(const std::string& name) const;

private:
    unsigned int m_id = 0;
    bool m_use_ubo = false;

    int compileShader(ShaderType type, const std::filesystem::path& path);
    int compileShader(ShaderType type, const char* source);
    void linkProgram(unsigned int vertex_shader, unsigned int fragment_shader);
    std::string combineFragmentShader(
        const char* template_source,
        const std::vector<ShaderPart>& parts
    );

};

}
