#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace glvx {

class Shader;
class UniformBuffer;

void check_opengl_errors();

// Drain all pending GL errors and report each of them.
// In debug builds this throws if any error was pending.
void check_opengl_errors_all();

// In debug builds every OpenGL call goes through GL_CALL and can be
// followed by an error check. Checking after every call costs one
// driver round trip (glGetError) per call and dominates debug-build
// CPU, so the default is per-frame: pending errors are drained once per
// frame when the window is displayed. Switch to per-call checking
// (slow, but attributes the error to the exact call) when debugging an
// OpenGL problem:
//     glvx::set_gl_error_checking(glvx::GlErrorChecking::PerCall);
enum class GlErrorChecking {
    PerFrame,
    PerCall
};
void set_gl_error_checking(GlErrorChecking checking);
GlErrorChecking get_gl_error_checking();

// Internal: true while per-call checking is enabled (debug builds).
extern bool gl_error_check_per_call;

bool has_active_gl_context();

namespace common {
    extern Shader* default_shader;
    extern Shader* subpixel_shader;
    extern UniformBuffer* uniform_buffer;
}

template<typename FuncType>
auto glCall(FuncType&& f) {
    if constexpr (std::is_same_v<decltype(f()), void>) {
        f();
        if (gl_error_check_per_call) {
            check_opengl_errors();
        }
    } else {
        auto result = f();
        if (gl_error_check_per_call) {
            check_opengl_errors();
        }
        return result;
    }
}

#ifdef NDEBUG
    #define GL_CALL(x) x
    #define GL_CALL_DEBUG(x)
#else
    #define GL_CALL(x) glCall([&] { return x; })
    #define GL_CALL_DEBUG(x) glCall([&] { return x; })
#endif

#define START_TRY try {
#define END_TRY \
    } catch (std::exception& e) { \
        throw std::runtime_error(__FUNCTION__": " + std::string(e.what())); \
    }

}
