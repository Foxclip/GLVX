#include "glvx/cursor.h"

namespace glvx {

Cursor::Cursor() { }

Cursor::~Cursor() {
    if (m_glfw_cursor) {
        glfwDestroyCursor(m_glfw_cursor);
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

    if (m_glfw_cursor) {
        glfwDestroyCursor(m_glfw_cursor);
    }

    m_glfw_cursor = glfwCreateStandardCursor(glfw_type);
    return m_glfw_cursor != nullptr;
}

}
