#include "glvis/mouse.h"
#include "glvis/window.h"

namespace glvis {

std::unordered_map<GLFWwindow*, Mouse::MouseState> Mouse::window_states;

bool Mouse::isButtonPressed(Mouse::Button button, const Window& window) {
    auto it = window_states.find(window.getWindowHandle());
    if (it != window_states.end()) {
        size_t idx = static_cast<size_t>(button);
        if (idx >= static_cast<size_t>(Mouse::Button::Count)) {
            return false;
        }
        return it->second.button_states[idx];
    }
    return false;
}

Vector2i Mouse::getPosition(const Window& window) {
    auto it = window_states.find(window.getWindowHandle());
    if (it != window_states.end()) {
        return it->second.position;
    }
    return Vector2i{0, 0};
}

void Mouse::setPosition(const Window& window, const Vector2i& position) {
    glfwSetCursorPos(window.getWindowHandle(), position.x, position.y);
    updatePosition(window.getWindowHandle(), position.x, position.y);
}

void Mouse::updatePosition(GLFWwindow* handle, double x, double y) {
    auto& state = window_states[handle];
    state.position = Vector2i(static_cast<int>(x), static_cast<int>(y));
    int wx, wy;
    glfwGetWindowPos(handle, &wx, &wy);
    state.global_position = Vector2i(static_cast<int>(x) + wx, static_cast<int>(y) + wy);
}

void Mouse::setButtonState(GLFWwindow* handle, Mouse::Button button, bool pressed) {
    size_t idx = static_cast<size_t>(button);
    if (idx < static_cast<size_t>(Mouse::Button::Count)) {
        window_states[handle].button_states[idx] = pressed;
    }
}

void Mouse::reset() {
    window_states.clear();
}

}
