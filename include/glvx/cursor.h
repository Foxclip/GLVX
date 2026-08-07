#pragma once

#include <GLFW/glfw3.h>

namespace glvx {

class Cursor {
public:
    enum class Type {
        Arrow,
        IBeam,
        Crosshair,
        Hand,
        ResizeHorizontal,
        ResizeVertical,
        ResizeDiagonal1,
        ResizeDiagonal2,
        ResizeAll,
    };

    Cursor();
    ~Cursor();

    bool loadFromSystem(Type type);

    friend class Window;

private:
    GLFWcursor* m_glfw_cursor = nullptr;
};

}
