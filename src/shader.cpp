#include "glvis/shader.h"
#include "glvis/utils.h"
#include <glad/glad.h>
#include "glvis/glvis_common.h"
#include <vector>
#include <cassert>
#include <sstream>
#include <algorithm>

namespace glvis {

Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, bool useUBO) : useUBO(useUBO) {
    START_TRY
    unsigned int vertexShader = compileShader(ShaderType::VERTEX, vertexPath);
    unsigned int fragmentShader = compileShader(ShaderType::FRAGMENT, fragmentPath);
    linkProgram(vertexShader, fragmentShader);
    END_TRY
}

Shader::Shader(const char* vertexSource, const char* fragmentSource, bool useUBO) : useUBO(useUBO) {
    START_TRY
    unsigned int vertexShader = compileShader(ShaderType::VERTEX, vertexSource);
    unsigned int fragmentShader = compileShader(ShaderType::FRAGMENT, fragmentSource);
    linkProgram(vertexShader, fragmentShader);
    END_TRY
}

Shader::Shader(
    const char* vertexSource,
    const char* fragmentTemplate,
    const std::vector<ShaderPart>& fragmentParts, bool useUBO)
: useUBO(useUBO) {
    START_TRY
    std::string combinedFrag = combineFragmentShader(fragmentTemplate, fragmentParts);
    unsigned int vertexShader = compileShader(ShaderType::VERTEX, vertexSource);
    unsigned int fragmentShader = compileShader(ShaderType::FRAGMENT, combinedFrag.c_str());
    linkProgram(vertexShader, fragmentShader);
    END_TRY
}

void Shader::use() {
    assert(ID != 0);
    GL_CALL(glUseProgram(ID));
}

void Shader::setBool(const std::string& name, bool value) const {
    assert(ID != 0);
    GLint loc = GL_CALL(glGetUniformLocation(ID, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform1i(loc, static_cast<int>(value)));
}

void Shader::setInt(const std::string& name, int value) const {
    assert(ID != 0);
    GLint loc = GL_CALL(glGetUniformLocation(ID, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform1i(loc, value));
}

void Shader::setFloat(const std::string& name, float value) const {
    assert(ID != 0);
    GLint loc = GL_CALL(glGetUniformLocation(ID, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform1f(loc, value));
}

void Shader::setVec3(const std::string &name, const Vector3& value) const {
    assert(ID != 0);
    GLint loc = GL_CALL(glGetUniformLocation(ID, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform3fv(loc, 1, &value.x));
}

void Shader::setVec4(const std::string &name, const Vector4& value) const {
    assert(ID != 0);
    GLint loc = GL_CALL(glGetUniformLocation(ID, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniform4fv(loc, 1, &value.x));
}

void Shader::setMat4(const std::string& name, const Matrix4& value) const {
    assert(ID != 0);
    GLint loc = GL_CALL(glGetUniformLocation(ID, name.c_str()));
    if (loc == -1) return;
    GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, value.getData()));
}

bool Shader::uniformExists(const std::string& name) const {
    assert(ID != 0);
    return GL_CALL(glGetUniformLocation(ID, name.c_str())) != -1;
}

void Shader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    START_TRY
    ID = GL_CALL(glCreateProgram());
    GL_CALL(glAttachShader(ID, vertexShader));
    GL_CALL(glAttachShader(ID, fragmentShader));
    GL_CALL(glLinkProgram(ID));
    int success;
    GL_CALL(glGetProgramiv(ID, GL_LINK_STATUS, &success));
    if (!success) {
        int length;
        GL_CALL(glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length));
        std::vector<char> infoLog(length);
        GL_CALL(glGetProgramInfoLog(ID, length, NULL, infoLog.data()));
        throw std::format("Linking failed\n{}", std::string(infoLog.data()));
    }

    GL_CALL(glDeleteShader(vertexShader));
    GL_CALL(glDeleteShader(fragmentShader));
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
        std::vector<char> infoLog(length);
        GL_CALL(glGetShaderInfoLog(shader, length, NULL, infoLog.data()));
        std::string typeStr = type == ShaderType::VERTEX ? "Vertex" : "Fragment";
        throw std::runtime_error(std::format("{} shader compilation failed\n{}", typeStr, std::string(infoLog.data())));
    }
    return shader;
    END_TRY
}

Shader::~Shader() {
    GL_CALL(glDeleteProgram(ID));
}

std::string Shader::combineFragmentShader(
    const char* templateSource,
    const std::vector<ShaderPart>& parts
) {
    std::istringstream stream(templateSource);
    std::vector<std::string> combined_lines;
    std::string line;
    while (std::getline(stream, line)) {
        combined_lines.push_back(line);
    }

    auto trim = [](const std::string& str) -> std::string {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    };

    ptrdiff_t include_line_index = -1;
    ptrdiff_t apply_line_index = -1;
    for (size_t i = 0; i < combined_lines.size(); i++) {
        std::string trimmed = trim(combined_lines[i]);
        if (trimmed == "%INCLUDE%") {
            include_line_index = static_cast<ptrdiff_t>(i);
        } else if (trimmed == "%APPLY%") {
            apply_line_index = static_cast<ptrdiff_t>(i);
        }
    }

    if (include_line_index == -1) {
        throw std::runtime_error(std::format("Unable to find %INCLUDE% in fragment template"));
    }
    if (apply_line_index == -1) {
        throw std::runtime_error(std::format("Unable to find %APPLY% in fragment template"));
    }

    if (parts.empty()) {
        combined_lines[include_line_index] = "// No shaders here";
        combined_lines[apply_line_index] = "    // No shaders here";
    } else {
        combined_lines.erase(combined_lines.begin() + include_line_index);
        size_t current_line_index = static_cast<size_t>(include_line_index);

        std::string eq;
        for (int i = 0; i < 32; i++) {
            eq += "=";
        }

        for (const auto& part : parts) {
            std::istringstream part_stream(part.source);
            std::vector<std::string> shader_lines;
            std::string pLine;
            while (std::getline(part_stream, pLine)) {
                shader_lines.push_back(pLine);
            }

            shader_lines.insert(shader_lines.begin(), "// " + eq + " BEGIN " + part.name + " " + eq);
            shader_lines.insert(shader_lines.end(), "// " + eq + " END " + part.name + " " + eq);

            combined_lines.insert(
                combined_lines.begin() + current_line_index,
                shader_lines.begin(),
                shader_lines.end()
            );
            current_line_index += shader_lines.size();
            apply_line_index += static_cast<ptrdiff_t>(shader_lines.size());
        }

        apply_line_index -= 1;

        combined_lines.erase(combined_lines.begin() + apply_line_index);
        size_t current_apply_index = static_cast<size_t>(apply_line_index);

        for (const auto& part : parts) {
            combined_lines.insert(
                combined_lines.begin() + current_apply_index,
                "    color = " + part.name + "_apply(color);"
            );
            current_apply_index++;
        }
    }

    std::string result;
    for (size_t i = 0; i < combined_lines.size(); i++) {
        if (i != 0) {
            result += "\n";
        }
        result += combined_lines[i];
    }

    return result;
}

}
