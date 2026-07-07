#include "glvis/glvis_common.h"

namespace glvis {

void check_opengl_errors() {
    START_TRY
    GLenum error = glGetError();
    std::string error_str;
    switch (error) {
        case GL_NO_ERROR:                      break;
        case GL_INVALID_ENUM:                  error_str = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error_str = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error_str = "GL_INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 error_str = "GL_OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error_str = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                               error_str = "Unknown OpenGL error";
    }
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error_str << std::endl;
#ifndef NDEBUG
        throw std::runtime_error("OpenGL error: " + error_str);
#endif
    }
    END_TRY
}

namespace common {
    Shader* defaultShader = nullptr;
    Shader* subpixelShader = nullptr;
    UniformBuffer* uniformBuffer = nullptr;
}

}
