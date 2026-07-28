#pragma once

#include "glvis/mouse.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <unordered_map>

namespace glvis {

class MousePosition {
public:
    static glm::ivec2 getPosition();
    static glm::ivec2 getPosition(GLFWwindow* window);
    static glm::ivec2 getGlobalPosition();
    static glm::ivec2 getGlobalPosition(GLFWwindow* window);
    static bool isButtonPressed(mouse::Button button);
    static bool isButtonPressed(mouse::Button button, GLFWwindow* window);

    static void setPosition(double x, double y);
    static void setPositionForWindow(GLFWwindow* handle, double x, double y);
    static void setButtonState(mouse::Button button, bool pressed);
    static void setButtonStateForWindow(GLFWwindow* handle, mouse::Button button, bool pressed);
    static void reset();

private:
    struct MouseState {
        glm::ivec2 position = {0, 0};
        glm::ivec2 global_position = {0, 0};
        bool button_states[static_cast<size_t>(mouse::Button::Count)] = {};
    };

    static MouseState default_state;
    static std::unordered_map<void*, MouseState> window_states;
};

}
