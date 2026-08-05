#pragma once

#include "glvx_tests/glvx_tests_common.h"
#include "glvx/window.h"
#include "glvx/rectangle.h"

using namespace glvx;

static const int TEST_APP_WINDOW_WIDTH = 100;
static const int TEST_APP_WINDOW_HEIGHT = 100;
static const Vector2i TEST_APP_RECT_SIZE = Vector2i(10, 10);
static const Color TEST_APP_CLEAR_COLOR = Color(64, 128, 255);

class TestApplication {
public:

    TestApplication();

    void init();
    void advance();
    glvx::Image readPixels();
    void toggleRectangle();

private:
    void process_input();
    void render();

    Window m_window;
    View m_view;
    Rectangle m_rectangle;
    bool m_draw_rectangle = false;
};

class ApplicationTestsModule : public test::TestModule {
public:
    ApplicationTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void initTest(test::Test& test);
    void clearTest(test::Test& test);
    void rectangleTest(test::Test& test);

private:

};

extern TestApplication app;
