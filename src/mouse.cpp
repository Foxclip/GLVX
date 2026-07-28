#include "glvis/mouse.h"

namespace glvis {

GLFWwindow* Mouse::current_window = nullptr;
std::unordered_map<void*, Mouse::MouseState> Mouse::window_states;

Vector2i Mouse::getPosition() {
    if (current_window) {
        auto it = window_states.find(current_window);
        if (it != window_states.end())
            return it->second.global_position;
    }
    return Vector2i{0, 0};
}

Vector2i Mouse::getPosition(GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        return it->second.position;
    }
    return Vector2i{0, 0};
}

bool Mouse::isButtonPressed(Mouse::Button button) {
    size_t idx = static_cast<size_t>(button);
    if (idx >= static_cast<size_t>(Mouse::Button::Count)) return false;
    if (current_window) {
        auto it = window_states.find(current_window);
        if (it != window_states.end())
            return it->second.button_states[idx];
    }
    return false;
}

bool Mouse::isButtonPressed(Mouse::Button button, GLFWwindow* window) {
    auto it = window_states.find(window);
    if (it != window_states.end()) {
        size_t idx = static_cast<size_t>(button);
        if (idx >= static_cast<size_t>(Mouse::Button::Count)) return false;
        return it->second.button_states[idx];
    }
    return false;
}

void Mouse::setPosition(GLFWwindow* handle, double x, double y) {
    current_window = handle;
    auto& state = window_states[handle];
    state.position = Vector2i(static_cast<int>(x), static_cast<int>(y));
    int wx, wy;
    glfwGetWindowPos(handle, &wx, &wy);
    state.global_position = Vector2i(static_cast<int>(x) + wx, static_cast<int>(y) + wy);
}

void Mouse::setButtonState(GLFWwindow* handle, Mouse::Button button, bool pressed) {
    current_window = handle;
    size_t idx = static_cast<size_t>(button);
    if (idx < static_cast<size_t>(Mouse::Button::Count)) {
        window_states[handle].button_states[idx] = pressed;
    }
}

void Mouse::reset() {
    current_window = nullptr;
    window_states.clear();
}

}
