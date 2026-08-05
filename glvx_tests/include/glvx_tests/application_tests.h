#pragma once

#include "glvx_tests/glvx_tests_common.h"
#include "glvx/window.h"
#include "glvx/rectangle.h"

using namespace glvx;

static const int TEST_APP_WINDOW_WIDTH = 100;
static const int TEST_APP_WINDOW_HEIGHT = 100;
static const Color TEST_APP_CLEAR_COLOR = Color(64, 128, 255);

class TestApplication {
public:

    TestApplication();

    void init();
    void advance();
    glvx::Image readPixels();

private:
    void process_input();
    void render();

    glvx::Window m_window;
    glvx::View m_view;
    glvx::Rectangle m_rectangle;
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

private:

};

extern TestApplication app;
