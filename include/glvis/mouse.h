#pragma once

#include <glvis/vector.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace glvis {

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

    static Vector2i getPosition();
    static Vector2i getPosition(GLFWwindow* window);
    static Vector2i getGlobalPosition();
    static Vector2i getGlobalPosition(GLFWwindow* window);
    static bool isButtonPressed(Button button);
    static bool isButtonPressed(Button button, GLFWwindow* window);

private:
    friend class Window;

    struct MouseState {
        Vector2i position = {0, 0};
        Vector2i global_position = {0, 0};
        bool button_states[static_cast<size_t>(Button::Count)] = {};
    };

    static MouseState default_state;
    static std::unordered_map<void*, MouseState> window_states;

    static void setPosition(double x, double y);
    static void setPositionForWindow(GLFWwindow* handle, double x, double y);
    static void setButtonState(Button button, bool pressed);
    static void setButtonStateForWindow(GLFWwindow* handle, Button button, bool pressed);
    static void reset();
};

}
