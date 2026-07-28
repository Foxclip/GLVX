#include "glvis/mouse.h"

namespace glvis {

Mouse::MouseState Mouse::default_state;
std::unordered_map<void*, Mouse::MouseState> Mouse::window_states;

glm::ivec2 Mouse::getPosition() {
    return default_state.position;
}

glm::ivec2 Mouse::getPosition(GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        return it->second.position;
    }
    return default_state.position;
}

glm::ivec2 Mouse::getGlobalPosition() {
    return default_state.global_position;
}

glm::ivec2 Mouse::getGlobalPosition(GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        return it->second.global_position;
    }
    return default_state.global_position;
}

bool Mouse::isButtonPressed(Mouse::Button button) {
    size_t idx = static_cast<size_t>(button);
    if (idx >= static_cast<size_t>(Mouse::Button::Count)) return false;
    return default_state.button_states[idx];
}

bool Mouse::isButtonPressed(Mouse::Button button, GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        size_t idx = static_cast<size_t>(button);
        if (idx >= static_cast<size_t>(Mouse::Button::Count)) return false;
        return it->second.button_states[idx];
    }
    return isButtonPressed(button);
}

void Mouse::setPosition(double x, double y) {
    default_state.position = glm::ivec2(static_cast<int>(x), static_cast<int>(y));
}

void Mouse::setPositionForWindow(GLFWwindow* handle, double x, double y) {
    auto& state = window_states[handle];
    state.position = glm::ivec2(static_cast<int>(x), static_cast<int>(y));
    int wx, wy;
    glfwGetWindowPos(handle, &wx, &wy);
    state.global_position = glm::ivec2(static_cast<int>(x) + wx, static_cast<int>(y) + wy);
}

void Mouse::setButtonState(Mouse::Button button, bool pressed) {
    size_t idx = static_cast<size_t>(button);
    if (idx < static_cast<size_t>(Mouse::Button::Count)) {
        default_state.button_states[idx] = pressed;
    }
}

void Mouse::setButtonStateForWindow(GLFWwindow* handle, Mouse::Button button, bool pressed) {
    size_t idx = static_cast<size_t>(button);
    if (idx < static_cast<size_t>(Mouse::Button::Count)) {
        window_states[handle].button_states[idx] = pressed;
    }
}

void Mouse::reset() {
    default_state = MouseState();
    window_states.clear();
}

}
