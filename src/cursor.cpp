#include "glvx/cursor.h"

namespace glvx {

Cursor::Cursor() { }

Cursor::~Cursor() {
    if (glfw_cursor) {
        glfwDestroyCursor(glfw_cursor);
    }
}

bool Cursor::loadFromSystem(Type type) {
    int glfw_type = 0;
    switch (type) {
        case Type::Arrow:            glfw_type = GLFW_ARROW_CURSOR;       break;
        case Type::IBeam:            glfw_type = GLFW_IBEAM_CURSOR;       break;
        case Type::Crosshair:        glfw_type = GLFW_CROSSHAIR_CURSOR;   break;
        case Type::Hand:             glfw_type = GLFW_HAND_CURSOR;        break;
        case Type::ResizeHorizontal: glfw_type = GLFW_RESIZE_EW_CURSOR;   break;
        case Type::ResizeVertical:   glfw_type = GLFW_RESIZE_NS_CURSOR;   break;
        case Type::ResizeDiagonal1:  glfw_type = GLFW_RESIZE_NWSE_CURSOR; break;
        case Type::ResizeDiagonal2:  glfw_type = GLFW_RESIZE_NESW_CURSOR; break;
        case Type::ResizeAll:        glfw_type = GLFW_RESIZE_ALL_CURSOR;  break;
    }

    if (glfw_cursor) {
        glfwDestroyCursor(glfw_cursor);
    }

    glfw_cursor = glfwCreateStandardCursor(glfw_type);
    return glfw_cursor != nullptr;
}

}
