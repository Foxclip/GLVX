#pragma once

#include <glvx/vector.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace glvx {

class Window;

class Mouse {
public:
    enum class Button {
        Left = 0,
        Right = 1,
        Middle = 2,
        XButton1 = 3,
        XButton2 = 4,
        Count = 5
    };

    static bool isButtonPressed(const Window& window, Button button);
    static Vector2i getPosition(const Window& window);
    static void setPosition(const Window& window, const Vector2i& position);

private:
    friend class Window;

    struct MouseState {
        bool button_states[static_cast<size_t>(Button::Count)] = {};
    };

    static std::unordered_map<GLFWwindow*, MouseState> window_states;

    static void setButtonState(GLFWwindow* handle, Button button, bool pressed);
    static void reset();
};

}
