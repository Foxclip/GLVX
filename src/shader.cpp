#include "glvx/shader.h"
#include "glvx/utils.h"
#include <glad/glad.h>
#include "glvx/glvx_common.h"
#include <vector>
#include <cassert>
#include <sstream>
#include <algorithm>

namespace glvx {

Shader::Shader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path, bool use_ubo) : m_use_ubo(use_ubo) {
    START_TRY
    unsigned int vertex_shader = compileShader(ShaderType::VERTEX, vertex_path);
    unsigned int fragment_shader = compileShader(ShaderType::FRAGMENT, fragment_path);
    linkProgram(vertex_shader, fragment_shader);
    END_TRY
}

Shader::Shader(const char* vertex_source, const char* fragment_source, bool use_ubo) : m_use_ubo(use_ubo) {
    START_TRY
    unsigned int vertex_shader = compileShader(ShaderType::VERTEX, vertex_source);
    unsigned int fragment_shader = compileShader(ShaderType::FRAGMENT, fragment_source);
    linkProgram(vertex_shader, fragment_shader);
    END_TRY
}

Shader::Shader(
    const char* vertex_source,
    const char* fragment_template,
    const std::vector<ShaderPart>& fragment_parts, bool use_ubo)
: m_use_ubo(use_ubo) {
    START_TRY
    std::string combined_frag = combineFragmentShader(fragment_template, fragment_parts);
    unsigned int vertex_shader = compileShader(ShaderType::VERTEX, vertex_source);
    unsigned int fragment_shader = compileShader(ShaderType::FRAGMENT, combined_frag.c_str());
    linkProgram(vertex_shader, fragment_shader);
    END_TRY
}

unsigned int Shader::getId() const {
    return m_id;
}

bool Shader::isUsingUBO() const {
    return m_use_ubo;
}

void Shader::use() {
    assert(m_id != 0);
    GL_CALL(glUseProgram(m_id));
}

void Shader::setBool(const std::string& name, bool value) const {
    assert(m_id != 0);
    GLint loc = GL_CALL(glGetUniformLocation(m_id, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform1i(loc, static_cast<int>(value)));
}

void Shader::setInt(const std::string& name, int value) const {
    assert(m_id != 0);
    GLint loc = GL_CALL(glGetUniformLocation(m_id, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform1i(loc, value));
}

void Shader::setFloat(const std::string& name, float value) const {
    assert(m_id != 0);
    GLint loc = GL_CALL(glGetUniformLocation(m_id, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform1f(loc, value));
}

void Shader::setVec3(const std::string &name, const Vector3& value) const {
    assert(m_id != 0);
    GLint loc = GL_CALL(glGetUniformLocation(m_id, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform3fv(loc, 1, &value.x));
}

void Shader::setVec4(const std::string &name, const Vector4& value) const {
    assert(m_id != 0);
    GLint loc = GL_CALL(glGetUniformLocation(m_id, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform4fv(loc, 1, &value.x));
}

void Shader::setMat4(const std::string& name, const Matrix4& value) const {
    assert(m_id != 0);
    GLint loc = GL_CALL(glGetUniformLocation(m_id, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, value.getData()));
}

bool Shader::uniformExists(const std::string& name) const {
    assert(m_id != 0);
    return GL_CALL(glGetUniformLocation(m_id, name.c_str())) != -1;
}

void Shader::linkProgram(unsigned int vertex_shader, unsigned int fragment_shader) {
    START_TRY
    m_id = GL_CALL(glCreateProgram());
    GL_CALL(glAttachShader(m_id, vertex_shader));
    GL_CALL(glAttachShader(m_id, fragment_shader));
    GL_CALL(glLinkProgram(m_id));
    int success;
    GL_CALL(glGetProgramiv(m_id, GL_LINK_STATUS, &success));
    if (!success) {
        int length;
        GL_CALL(glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length));
        std::vector<char> info_log(length);
        GL_CALL(glGetProgramInfoLog(m_id, length, NULL, info_log.data()));
        throw std::format("Linking failed\n{}", std::string(info_log.data()));
    }

    GL_CALL(glDeleteShader(vertex_shader));
    GL_CALL(glDeleteShader(fragment_shader));
    END_TRY
}

int Shader::compileShader(ShaderType type, const std::filesystem::path& path) {
    return compileShader(type, file_to_str(path).c_str());
}

int Shader::compileShader(ShaderType type, const char* source) {
    START_TRY
    unsigned int shader = GL_CALL(glCreateShader(type == ShaderType::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER));
    GL_CALL(glShaderSource(shader, 1, &source, NULL));
    GL_CALL(glCompileShader(shader));
    int success;
    GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        int length;
        GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
        std::vector<char> info_log(length);
        GL_CALL(glGetShaderInfoLog(shader, length, NULL, info_log.data()));
        std::string type_str = type == ShaderType::VERTEX ? "Vertex" : "Fragment";
        throw std::runtime_error(std::format("{} shader compilation failed\n{}", type_str, std::string(info_log.data())));
    }
    return shader;
    END_TRY
}

Shader::~Shader() {
    if (!has_active_gl_context()) {
        return;
    }
    GL_CALL(glDeleteProgram(m_id));
}

std::string Shader::combineFragmentShader(
    const char* template_source,
    const std::vector<ShaderPart>& parts
) {
    auto trim = [](const std::string& str) -> std::string {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    };

    std::string separator(32, '=');

    // Build include block
    std::string include_block;
    if (parts.empty()) {
        include_block = "// No shaders here";
    } else {
        for (const auto& part : parts) {
            if (!include_block.empty()) include_block += '\n';
            include_block += "// " + separator + " BEGIN " + part.name + " " + separator;
            include_block += '\n';
            include_block += part.source;
            include_block += '\n';
            include_block += "// " + separator + " END " + part.name + " " + separator;
        }
    }

    // Build apply block
    std::string apply_block;
    if (parts.empty()) {
        apply_block = "    // No shaders here";
    } else {
        for (const auto& part : parts) {
            if (!apply_block.empty()) apply_block += '\n';
            apply_block += "    color = " + part.name + "_apply(color);";
        }
    }

    std::istringstream stream(template_source);
    std::string line;
    bool found_include = false, found_apply = false;
    std::string result;
    while (std::getline(stream, line)) {
        std::string trimmed = trim(line);
        if (trimmed == "%INCLUDE%") {
            found_include = true;
            if (!result.empty()) result += '\n';
            result += include_block;
        } else if (trimmed == "%APPLY%") {
            found_apply = true;
            if (!result.empty()) result += '\n';
            result += apply_block;
        } else {
            if (!result.empty()) result += '\n';
            result += line;
        }
    }
    if (!found_include)
        throw std::runtime_error(std::format("Unable to find %INCLUDE% in fragment template"));
    if (!found_apply)
        throw std::runtime_error(std::format("Unable to find %APPLY% in fragment template"));

    return result;
}

}
