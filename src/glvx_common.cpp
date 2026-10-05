#include "glvx/glvx_common.h"

namespace glvx {

bool gl_error_check_per_call = false;

void set_gl_error_checking(GlErrorChecking checking) {
    gl_error_check_per_call = (checking == GlErrorChecking::PerCall);
}

GlErrorChecking get_gl_error_checking() {
    return gl_error_check_per_call ? GlErrorChecking::PerCall : GlErrorChecking::PerFrame;
}

bool has_active_gl_context() {
    return glfwGetCurrentContext() != nullptr;
}

static const char* gl_error_name(GLenum error) {
    switch (error) {
        case GL_NO_ERROR:                      return "GL_NO_ERROR";
        case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:                               return "Unknown OpenGL error";
    }
}

void check_opengl_errors() {
    START_TRY
    GLenum error = glGetError();
    const char* error_str = gl_error_name(error);
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error_str << std::endl;
#ifndef NDEBUG
        throw std::runtime_error(std::string("OpenGL error: ") + error_str);
#endif
    }
    END_TRY
}

void check_opengl_errors_all() {
    START_TRY
    bool had_error = false;
    for (;;) {
        GLenum error = glGetError();
        if (error == GL_NO_ERROR) {
            break;
        }
        had_error = true;
        std::cerr << "OpenGL error: " << gl_error_name(error) << std::endl;
    }
#ifndef NDEBUG
    if (had_error) {
        throw std::runtime_error("OpenGL error(s) detected at frame boundary; enable glvx::GlErrorChecking::PerCall to attribute the error to a specific call");
    }
#endif
    END_TRY
}

namespace common {
    Shader* default_shader = nullptr;
    Shader* subpixel_shader = nullptr;
    UniformBuffer* uniform_buffer = nullptr;
}

}
