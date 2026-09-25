#pragma once

#include <GLFW/glfw3.h>
#include "glvx/vector.h"

namespace glvx {

class Window;

class Cursor {
public:
    // SFML-compatible system cursor types.
    //
    // Availability:
    // - Windows: all 21 types are native. ArrowWait/Wait/Help are copied from
    //   the Win32 system cursors (IDC_APPSTARTING/IDC_WAIT/IDC_HELP) at load
    //   time, keeping their real hotspots.
    // - Other platforms: the 18 common types are native; ArrowWait, Wait and
    //   Help fall back to the Arrow cursor.
    //
    // The 8 single-headed size types (SizeLeft/Right/Top/Bottom and the 4
    // single-corner types) are the double-headed system shapes, as on SFML's
    // Windows behavior.
    enum class Type {
        Arrow,
        ArrowWait,
        Wait,
        Text,
        Hand,
        SizeHorizontal,
        SizeVertical,
        SizeTopLeftBottomRight,
        SizeBottomLeftTopRight,
        SizeLeft,
        SizeRight,
        SizeTop,
        SizeBottom,
        SizeTopLeft,
        SizeBottomRight,
        SizeBottomLeft,
        SizeTopRight,
        SizeAll,
        Cross,
        Help,
        NotAllowed,
    };

    Cursor();
    ~Cursor();

    Cursor(const Cursor& other) = delete;
    Cursor& operator=(const Cursor& other) = delete;

    bool loadFromSystem(Type type);

    // pixels is an RGBA8888 buffer of size.x * size.y pixels.
    // Returns false (and leaves the cursor unchanged) if pixels is null or
    // size is zero.
    bool loadFromPixels(const unsigned char* pixels, Vector2u size, Vector2u hotspot);

    friend class Window;

private:
    GLFWcursor* m_glfw_cursor = nullptr;
};

}
