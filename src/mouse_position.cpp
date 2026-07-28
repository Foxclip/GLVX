#include "glvis/mouse_position.h"

namespace glvis {

MousePosition::MouseState MousePosition::default_state;
std::unordered_map<void*, MousePosition::MouseState> MousePosition::window_states;

glm::ivec2 MousePosition::getPosition() {
    return default_state.position;
}

glm::ivec2 MousePosition::getPosition(GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        return it->second.position;
    }
    return default_state.position;
}

glm::ivec2 MousePosition::getGlobalPosition() {
    return default_state.global_position;
}

glm::ivec2 MousePosition::getGlobalPosition(GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        return it->second.global_position;
    }
    return default_state.global_position;
}

bool MousePosition::isButtonPressed(mouse::Button button) {
    size_t idx = static_cast<size_t>(button);
    if (idx >= static_cast<size_t>(mouse::Button::Count)) return false;
    return default_state.button_states[idx];
}

bool MousePosition::isButtonPressed(mouse::Button button, GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        size_t idx = static_cast<size_t>(button);
        if (idx >= static_cast<size_t>(mouse::Button::Count)) return false;
        return it->second.button_states[idx];
    }
    return isButtonPressed(button);
}

void MousePosition::setPosition(double x, double y) {
    default_state.position = glm::ivec2(static_cast<int>(x), static_cast<int>(y));
}

void MousePosition::setPositionForWindow(GLFWwindow* handle, double x, double y) {
    auto& state = window_states[handle];
    state.position = glm::ivec2(static_cast<int>(x), static_cast<int>(y));
    int wx, wy;
    glfwGetWindowPos(handle, &wx, &wy);
    state.global_position = glm::ivec2(static_cast<int>(x) + wx, static_cast<int>(y) + wy);
}

void MousePosition::setButtonState(mouse::Button button, bool pressed) {
    size_t idx = static_cast<size_t>(button);
    if (idx < static_cast<size_t>(mouse::Button::Count)) {
        default_state.button_states[idx] = pressed;
    }
}

void MousePosition::setButtonStateForWindow(GLFWwindow* handle, mouse::Button button, bool pressed) {
    size_t idx = static_cast<size_t>(button);
    if (idx < static_cast<size_t>(mouse::Button::Count)) {
        window_states[handle].button_states[idx] = pressed;
    }
}

void MousePosition::reset() {
    default_state = MouseState();
    window_states.clear();
}

}
