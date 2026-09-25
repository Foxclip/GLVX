#include "glvx_tests/cursor_tests.h"
#include "glvx/cursor.h"

#include <array>
#include <vector>

namespace {

const std::array<Cursor::Type, 21> all_cursor_types = {
    Cursor::Type::Arrow,
    Cursor::Type::ArrowWait,
    Cursor::Type::Wait,
    Cursor::Type::Text,
    Cursor::Type::Hand,
    Cursor::Type::SizeHorizontal,
    Cursor::Type::SizeVertical,
    Cursor::Type::SizeTopLeftBottomRight,
    Cursor::Type::SizeBottomLeftTopRight,
    Cursor::Type::SizeLeft,
    Cursor::Type::SizeRight,
    Cursor::Type::SizeTop,
    Cursor::Type::SizeBottom,
    Cursor::Type::SizeTopLeft,
    Cursor::Type::SizeBottomRight,
    Cursor::Type::SizeBottomLeft,
    Cursor::Type::SizeTopRight,
    Cursor::Type::SizeAll,
    Cursor::Type::Cross,
    Cursor::Type::Help,
    Cursor::Type::NotAllowed,
};

std::vector<unsigned char> make_red_square(int size) {
    std::vector<unsigned char> pixels(size * size * 4);
    for (int i = 0; i < size * size; i++) {
        pixels[i * 4 + 0] = 255; // red
        pixels[i * 4 + 3] = 255; // alpha
    }
    return pixels;
}

}

CursorTestsModule::CursorTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto cursor_system_all_types_test = addTest("cursor_system_all_types", [&](test::Test& test) { cursorSystemAllTypesTest(test); });
    auto cursor_pixels_load_test = addTest("cursor_pixels_load", [&](test::Test& test) { cursorPixelsLoadTest(test); });
    auto cursor_pixels_invalid_test = addTest("cursor_pixels_invalid", [&](test::Test& test) { cursorPixelsInvalidTest(test); });
    auto cursor_set_on_window_test = addTest("cursor_set_on_window", { cursor_pixels_load_test }, [&](test::Test& test) { cursorSetOnWindowTest(test); });
}

void CursorTestsModule::beforeRunModule() {
    // the Application module closes its own window, which leaves no current
    // GL context on the thread; restore the shared window's context
    if (window.isOpen()) {
        glfwMakeContextCurrent(window.getWindowHandle());
    }
}

void CursorTestsModule::cursorSystemAllTypesTest(test::Test& test) {
    // one instance for all types also exercises the destroy/replace path
    Cursor cursor;
    for (Cursor::Type type : all_cursor_types) {
        T_CHECK(cursor.loadFromSystem(type));
    }
}

void CursorTestsModule::cursorPixelsLoadTest(test::Test& test) {
    std::vector<unsigned char> pixels = make_red_square(32);
    Cursor cursor;
    T_CHECK(cursor.loadFromPixels(pixels.data(), Vector2u(32, 32), Vector2u(2, 2)));
}

void CursorTestsModule::cursorPixelsInvalidTest(test::Test& test) {
    std::vector<unsigned char> pixels(4);
    Cursor cursor;
    T_CHECK(cursor.loadFromPixels(nullptr, Vector2u(1, 1), Vector2u(0, 0)) == false);
    T_CHECK(cursor.loadFromPixels(pixels.data(), Vector2u(0, 5), Vector2u(0, 0)) == false);
}

void CursorTestsModule::cursorSetOnWindowTest(test::Test& test) {
    // smoke test: no OS-level readback exists, so we only check nothing crashes
    Cursor cursor;
    std::vector<unsigned char> pixels = make_red_square(32);
    Cursor pixels_cursor;
    T_CHECK(pixels_cursor.loadFromPixels(pixels.data(), Vector2u(32, 32), Vector2u(2, 2)));

    T_CHECK(cursor.loadFromSystem(Cursor::Type::Arrow));
    window.setMouseCursor(cursor);
    window.display();

    T_CHECK(cursor.loadFromSystem(Cursor::Type::Wait));
    window.setMouseCursor(cursor);
    window.display();

    T_CHECK(cursor.loadFromSystem(Cursor::Type::Hand));
    window.setMouseCursor(cursor);
    window.display();

    T_CHECK(cursor.loadFromSystem(Cursor::Type::SizeLeft));
    window.setMouseCursor(cursor);
    window.display();

    window.setMouseCursor(pixels_cursor);
    window.display();
}

void CursorTestsModule::afterRunModule() {
    if (window.isOpen()) {
        Cursor cursor;
        if (cursor.loadFromSystem(Cursor::Type::Arrow)) {
            window.setMouseCursor(cursor);
            window.display();
        }
    }
}
