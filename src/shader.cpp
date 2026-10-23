#include "glvx/shader.h"
#include "glvx/utils.h"
#include <format>
#include <glad/glad.h>
#include "glvx/glvx_common.h"
#include <vector>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <regex>

namespace glvx {

namespace {

// "4.1.0 Core Profile ..." -> 410. Returns 0 when no context is current.
int shading_language_context_version() {
    const GLubyte* version_string = glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (version_string == nullptr) {
        return 0;
    }
    int major = 0;
    int minor = 0;
    std::sscanf(reinterpret_cast<const char*>(version_string), "%d.%d", &major, &minor);
    return major * 100 + minor;
}

// Removes the "instance." qualifier from member references
// (object.color -> color) so the rewritten plain uniforms can be used.
void strip_uniform_instance_qualifiers(std::string& text, const std::string& instance) {
    const std::string pattern = instance + ".";
    std::size_t search_from = 0;
    while (true) {
        const std::size_t pos = text.find(pattern, search_from);
        if (pos == std::string::npos) {
            return;
        }
        const char before = pos > 0 ? text[pos - 1] : ' ';
        const bool preceded_by_word_char =
            std::isalnum(static_cast<unsigned char>(before)) || before == '_';
        if (preceded_by_word_char) {
            search_from = pos + 1;
            continue;
        }
        text.erase(pos, instance.size() + 1); // removes the instance and the dot
        search_from = pos;
    }
}

// Rewrites uniform blocks (layout(binding = N) uniform Name { ... } instance;)
// into equivalent plain uniform declarations and strips the instance
// qualifier from member references. Needed on contexts below GLSL 420, where
// the GLSL compiler rejects the binding layout qualifier even with the ARB
// extension enabled.
std::string replace_uniform_blocks(const std::string& source) {
    static const std::regex block_pattern(
        R"(layout\s*\(\s*binding\s*=\s*\d+\s*\)\s*uniform\s+[A-Za-z_]\w*\s*\{([^{}]*)\}\s*([A-Za-z_]\w*)\s*;)"
    );
    std::string result;
    result.reserve(source.size());
    std::string rest = source;
    while (true) {
        std::cmatch match;
        if (!std::regex_search(rest.c_str(), match, block_pattern)) {
            break;
        }
        const std::string instance(match[2].first, match[2].second);
        std::string plain_declarations;
        std::istringstream members_stream(match[1]);
        std::string member;
        while (std::getline(members_stream, member, ';')) {
            const auto begin = member.find_first_not_of(" \t\r\n");
            if (begin == std::string::npos) {
                continue;
            }
            const auto end = member.find_last_not_of(" \t\r\n");
            plain_declarations += "uniform " + member.substr(begin, end - begin + 1) + ";\n";
        }
        result.append(rest, 0, match.position(0));
        result += plain_declarations;
        std::string tail(rest.begin() + match.position(0) + match.length(0), rest.end());
        strip_uniform_instance_qualifiers(tail, instance);
        rest = std::move(tail);
    }
    result += rest;
    return result;
}

// GLSL sources carry the version they were written for. When the context's
// maximum supported version is lower (e.g. #version 420 core on a GL 4.1
// driver), the source is rewritten so it still compiles: the #version number
// is written down, and uniform blocks are converted to plain uniforms (the
// GLSL compiler here rejects the binding layout qualifier below 420).
std::string adapt_shading_language_version(const char* source) {
    const int context_version = shading_language_context_version();
    const std::string src(source);
    if (context_version == 0) {
        return src;
    }
    std::string result = src;
    const std::string directive = "#version";
    const std::size_t directive_pos = result.find(directive);
    if (directive_pos != std::string::npos) {
        std::size_t number_start = directive_pos + directive.size();
        while (number_start < result.size() && std::isspace(static_cast<unsigned char>(result[number_start]))) {
            ++number_start;
        }
        if (number_start + 3 <= result.size() &&
            std::isdigit(static_cast<unsigned char>(result[number_start])) &&
            std::isdigit(static_cast<unsigned char>(result[number_start + 1])) &&
            std::isdigit(static_cast<unsigned char>(result[number_start + 2]))) {
            const int requested = std::stoi(result.substr(number_start, 3));
            if (requested > context_version) {
                // GLSL versions are 3-digit numbers, so the replacement keeps
                // all offsets valid.
                char replaced[16];
                std::snprintf(replaced, sizeof(replaced), "%03d", context_version);
                result.replace(number_start, 3, replaced);
            }
        }
    }
    if (context_version < 420) {
        result = replace_uniform_blocks(result);
    }
    return result;
}

} // namespace

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
    // On contexts below GLSL 420 the uniform blocks in the source are
    // compiled as plain uniforms, so the UBO draw path is unusable even when
    // the shader was created with use_ubo=true.
    return m_use_ubo && m_ubo_available;
}

void Shader::use() {
    assert(m_id != 0);
    GL_CALL(glUseProgram(m_id));
}

GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = m_uniform_locations.find(name);
    if (it != m_uniform_locations.end()) {
        return it->second;
    }
    // Not an active uniform (optimized away, or not in the linked program).
    // Cache the -1 result so it is queried only once.
    GLint loc = GL_CALL(glGetUniformLocation(m_id, name.c_str()));
    m_uniform_locations.emplace(name, loc);
    return loc;
}

void Shader::setBool(const std::string& name, bool value) const {
    assert(m_id != 0);
    GLint loc = getUniformLocation(name);
    if (loc == -1) return;
    GL_CALL(glUniform1i(loc, static_cast<int>(value)));
}

void Shader::setInt(const std::string& name, int value) const {
    assert(m_id != 0);
    GLint loc = getUniformLocation(name);
    if (loc == -1) return;
    GL_CALL(glUniform1i(loc, value));
}

void Shader::setFloat(const std::string& name, float value) const {
    assert(m_id != 0);
    GLint loc = getUniformLocation(name);
    if (loc == -1) return;
    GL_CALL(glUniform1f(loc, value));
}

void Shader::setVec3(const std::string &name, const Vector3& value) const {
    assert(m_id != 0);
    GLint loc = getUniformLocation(name);
    if (loc == -1) return;
    GL_CALL(glUniform3fv(loc, 1, &value.x));
}

void Shader::setVec4(const std::string &name, const Vector4& value) const {
    assert(m_id != 0);
    GLint loc = getUniformLocation(name);
    if (loc == -1) return;
    GL_CALL(glUniform4fv(loc, 1, &value.x));
}

void Shader::setMat4(const std::string& name, const Matrix4& value) const {
    assert(m_id != 0);
    GLint loc = getUniformLocation(name);
    if (loc == -1) return;
    GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, value.getData()));
}

bool Shader::uniformExists(const std::string& name) const {
    assert(m_id != 0);
    return getUniformLocation(name) != -1;
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
    m_ubo_available = shading_language_context_version() >= 420;
    cacheUniformLocations();
    END_TRY
}

void Shader::cacheUniformLocations() {
    int uniform_count = 0;
    GL_CALL(glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &uniform_count));
    for (int i = 0; i < uniform_count; i++) {
        GLsizei name_length = 0;
        GLint uniform_size = 0;
        GLenum uniform_type = GL_ZERO;
        GL_CALL(glGetActiveUniform(m_id, (GLuint)i, 0, &name_length, &uniform_size, &uniform_type, nullptr));
        std::vector<char> name(name_length + 1);
        GL_CALL(glGetActiveUniform(m_id, (GLuint)i, (GLsizei)name.size(), &name_length, &uniform_size, &uniform_type, name.data()));
        m_uniform_locations.emplace(std::string(name.data()), GL_CALL(glGetUniformLocation(m_id, name.data())));
    }
}

int Shader::compileShader(ShaderType type, const std::filesystem::path& path) {
    return compileShader(type, file_to_str(path).c_str());
}

int Shader::compileShader(ShaderType type, const char* source) {
    START_TRY
    const std::string adapted_source = adapt_shading_language_version(source);
    { // TEMP DEBUG — remove before finishing
        FILE* dbg = std::fopen("/tmp/dbg_shader.txt", "a");
        if (dbg) {
            std::fprintf(dbg, "===== %s =====\n%s\n", type == ShaderType::VERTEX ? "VERT" : "FRAG", adapted_source.c_str());
            std::fclose(dbg);
        }
    }
    const char* shader_source = adapted_source.c_str();
    unsigned int shader = GL_CALL(glCreateShader(type == ShaderType::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER));
    GL_CALL(glShaderSource(shader, 1, &shader_source, NULL));
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
