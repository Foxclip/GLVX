#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <unordered_map>

namespace glvis {
namespace mouse {

enum class Button {
    Left = 0,
    Right = 1,
    Middle = 2,
    XButton1 = 3,
    XButton2 = 4,
    Count = 5
};

}

class Mouse {
public:
    static glm::ivec2 getPosition();
    static glm::ivec2 getPosition(GLFWwindow* window);
    static glm::ivec2 getGlobalPosition();
    static glm::ivec2 getGlobalPosition(GLFWwindow* window);
    static bool isButtonPressed(mouse::Button button);
    static bool isButtonPressed(mouse::Button button, GLFWwindow* window);

private:
    friend class Window;

    struct MouseState {
        glm::ivec2 position = {0, 0};
        glm::ivec2 global_position = {0, 0};
        bool button_states[static_cast<size_t>(mouse::Button::Count)] = {};
    };

    static MouseState default_state;
    static std::unordered_map<void*, MouseState> window_states;

    static void setPosition(double x, double y);
    static void setPositionForWindow(GLFWwindow* handle, double x, double y);
    static void setButtonState(mouse::Button button, bool pressed);
    static void setButtonStateForWindow(GLFWwindow* handle, mouse::Button button, bool pressed);
    static void reset();
};

}
