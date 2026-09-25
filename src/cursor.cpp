#include "glvx/cursor.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <cstdint>
#include <utility>
#include <vector>

namespace glvx {

#ifdef _WIN32
// Copies a Win32 system cursor to RGBA and creates a GLFWcursor.
// cursor_id is the resource id (IDC_* value), used in MAKEINTRESOURCE form.
// The hotspot comes from GetIconInfo (the cursor's real hotspot).
// Returns false if any step fails (caller falls back to arrow).
static bool createSystemCursorWin32(DWORD cursor_id, GLFWcursor** out_cursor) {
    HCURSOR h_cursor = LoadCursorW(NULL, (PCWSTR)(uintptr_t)cursor_id);
    if (!h_cursor) {
        return false;
    }

    ICONINFO icon_info = {};
    if (!GetIconInfo(h_cursor, &icon_info)) {
        return false;
    }

    if (icon_info.hbmColor == NULL) {
        // monochrome cursors have no color bitmap
        DeleteObject(icon_info.hbmMask);
        return false;
    }

    BITMAP bitmap = {};
    if (!GetObjectW(icon_info.hbmColor, sizeof(BITMAP), &bitmap)) {
        DeleteObject(icon_info.hbmColor);
        DeleteObject(icon_info.hbmMask);
        return false;
    }

    const int width = bitmap.bmWidth;
    const int height = bitmap.bmHeight;
    std::vector<unsigned char> pixels(width * height * 4);

    HDC dc = GetDC(NULL);
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = width;
    bitmap_info.bmiHeader.biHeight = -height; // top-down
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    GetDIBits(dc, icon_info.hbmColor, 0, height, pixels.data(), &bitmap_info, DIB_RGB_COLORS);
    ReleaseDC(NULL, dc);

    DeleteObject(icon_info.hbmColor);
    DeleteObject(icon_info.hbmMask);

    // GetDIBits yields B,G,R,A memory bytes; GLFWimage expects R,G,B,A
    for (int i = 0; i < width * height; i++) {
        std::swap(pixels[i * 4], pixels[i * 4 + 2]);
    }

    GLFWimage image = {};
    image.width = width;
    image.height = height;
    image.pixels = pixels.data();

    *out_cursor = glfwCreateCursor(&image, (int)icon_info.xHotspot, (int)icon_info.yHotspot);
    return *out_cursor != nullptr;
}
#endif

Cursor::Cursor() { }

Cursor::~Cursor() {
    if (m_glfw_cursor) {
        glfwDestroyCursor(m_glfw_cursor);
    }
}

bool Cursor::loadFromSystem(Type type) {
    GLFWcursor* glfw_cursor = nullptr;

#ifdef _WIN32
    // Types without a GLFW standard shape are copied from the native
    // Win32 system cursors.
    switch (type) {
        case Type::ArrowWait:
            createSystemCursorWin32((DWORD)(uintptr_t)IDC_APPSTARTING, &glfw_cursor);
            break;
        case Type::Wait:
            createSystemCursorWin32((DWORD)(uintptr_t)IDC_WAIT, &glfw_cursor);
            break;
        case Type::Help:
            createSystemCursorWin32((DWORD)(uintptr_t)IDC_HELP, &glfw_cursor);
            break;
        default:
            break;
    }
#endif

    if (!glfw_cursor) {
        // 18 common types on all platforms; on non-Windows the 3 special
        // types fall back to the arrow cursor, as does a failed extraction.
        int glfw_type = GLFW_ARROW_CURSOR;
        switch (type) {
            case Type::Arrow:                   glfw_type = GLFW_ARROW_CURSOR;         break;
            case Type::Text:                    glfw_type = GLFW_IBEAM_CURSOR;         break;
            case Type::Hand:                    glfw_type = GLFW_POINTING_HAND_CURSOR; break;
            case Type::SizeHorizontal:          glfw_type = GLFW_RESIZE_EW_CURSOR;     break;
            case Type::SizeVertical:            glfw_type = GLFW_RESIZE_NS_CURSOR;     break;
            case Type::SizeTopLeftBottomRight:  glfw_type = GLFW_RESIZE_NWSE_CURSOR;   break;
            case Type::SizeBottomLeftTopRight:  glfw_type = GLFW_RESIZE_NESW_CURSOR;   break;
            case Type::SizeLeft:                glfw_type = GLFW_RESIZE_EW_CURSOR;     break;
            case Type::SizeRight:               glfw_type = GLFW_RESIZE_EW_CURSOR;     break;
            case Type::SizeTop:                 glfw_type = GLFW_RESIZE_NS_CURSOR;     break;
            case Type::SizeBottom:              glfw_type = GLFW_RESIZE_NS_CURSOR;     break;
            case Type::SizeTopLeft:             glfw_type = GLFW_RESIZE_NWSE_CURSOR;   break;
            case Type::SizeBottomRight:         glfw_type = GLFW_RESIZE_NWSE_CURSOR;   break;
            case Type::SizeBottomLeft:          glfw_type = GLFW_RESIZE_NESW_CURSOR;   break;
            case Type::SizeTopRight:            glfw_type = GLFW_RESIZE_NESW_CURSOR;   break;
            case Type::SizeAll:                 glfw_type = GLFW_RESIZE_ALL_CURSOR;    break;
            case Type::Cross:                   glfw_type = GLFW_CROSSHAIR_CURSOR;     break;
            case Type::NotAllowed:              glfw_type = GLFW_NOT_ALLOWED_CURSOR;   break;
            default:                            break;
        }
        glfw_cursor = glfwCreateStandardCursor(glfw_type);
    }

    if (m_glfw_cursor) {
        glfwDestroyCursor(m_glfw_cursor);
    }

    m_glfw_cursor = glfw_cursor;
    return m_glfw_cursor != nullptr;
}

bool Cursor::loadFromPixels(const unsigned char* pixels, Vector2u size, Vector2u hotspot) {
    if (!pixels || size.x == 0 || size.y == 0) {
        return false;
    }

    if (m_glfw_cursor) {
        glfwDestroyCursor(m_glfw_cursor);
    }

    GLFWimage image = {};
    image.width = size.x;
    image.height = size.y;
    image.pixels = const_cast<unsigned char*>(pixels);

    m_glfw_cursor = glfwCreateCursor(&image, hotspot.x, hotspot.y);
    return m_glfw_cursor != nullptr;
}

}
