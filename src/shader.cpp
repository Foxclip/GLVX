#include "glvis/shader.h"
#include "glvis/utils.h"
#include <glad/glad.h>
#include "glvis/glvis_common.h"

namespace glvis {

Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    START_TRY
    unsigned int vertexShader = compileShader(ShaderType::VERTEX, vertexPath);
    unsigned int fragmentShader = compileShader(ShaderType::FRAGMENT, fragmentPath);
    ID = GL_CALL(glCreateProgram());
    GL_CALL(glAttachShader(ID, vertexShader));
    GL_CALL(glAttachShader(ID, fragmentShader));
    GL_CALL(glLinkProgram(ID));
    int success;
    GL_CALL(glGetProgramiv(ID, GL_LINK_STATUS, &success));
    if (!success) {
        char infoLog[8192];
        GL_CALL(glGetProgramInfoLog(ID, 8192, NULL, infoLog));
        throw std::format("Linking failed\n{}", infoLog);
    }
    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));
    END_TRY
}

void Shader::use() {
    GL_CALL(glUseProgram(ID));
}

void Shader::setBool(const std::string& name, bool value) const {
    GL_CALL(glUniform1i(GL_CALL(glGetUniformLocation(ID, name.c_str())), (int)value));
}

void Shader::setInt(const std::string& name, int value) const {
    GL_CALL(glUniform1i(GL_CALL(glGetUniformLocation(ID, name.c_str())), (int)value));
}

void Shader::setFloat(const std::string& name, float value) const {
    GL_CALL(glUniform1f(GL_CALL(glGetUniformLocation(ID, name.c_str())), value));
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    GL_CALL(glUniform3fv(GL_CALL(glGetUniformLocation(ID, name.c_str())), 1, glm::value_ptr(value)));
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    GL_CALL(glUniform4fv(GL_CALL(glGetUniformLocation(ID, name.c_str())), 1, glm::value_ptr(value)));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    GL_CALL(glUniformMatrix4fv(GL_CALL(glGetUniformLocation(ID, name.c_str())), 1, GL_FALSE, glm::value_ptr(value)));
}

int Shader::compileShader(ShaderType type, const std::filesystem::path& path) {
    START_TRY
    std::string source = file_to_str(path);
    const char* sourceCstr = source.c_str();
    unsigned int shader = GL_CALL(glCreateShader(type == ShaderType::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER));
    GL_CALL(glShaderSource(shader, 1, &sourceCstr, NULL));
    GL_CALL(glCompileShader(shader));
    int success;
    char infoLog[8192];
    GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        GL_CALL(glGetShaderInfoLog(shader, 8192, NULL, infoLog));
        std::string typeStr = type == ShaderType::VERTEX ? "Vertex" : "Fragment";
        throw std::runtime_error(std::format("{} shader compilation failed: {}\n{}", typeStr, path.string(), infoLog));
    }
    return shader;
    END_TRY
}

Shader::~Shader() {
    GL_CALL(glDeleteProgram(ID));
}

}
