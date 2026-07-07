#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace glvis {

class Shader;
class UniformBuffer;

void check_opengl_errors();

namespace common {
    extern Shader* defaultShader;
    extern Shader* subpixelShader;
    extern UniformBuffer* uniformBuffer;
}

template<typename FuncType>
auto glCall(FuncType&& f) {
    if constexpr (std::is_same_v<decltype(f()), void>) {
        f();
        check_opengl_errors();
    } else {
        auto result = f();
        check_opengl_errors();
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
