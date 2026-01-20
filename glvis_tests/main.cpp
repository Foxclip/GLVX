#include <iostream>
#include "test_lib/test.h"
#include "glvis/vector.h"
#include "glvis/color.h"
#include "glvis/vertex.h"
#include "glvis/rectangle.h"
#include "glvis/circle.h"
#include "glvis/window.h"
#include "glvis/texture.h"

using namespace glvis;

class GlvisTestModule : public test::TestModule {
public:
    GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes = { });

private:
    void basicTest(test::Test& test);
    void clearTest(test::Test& test);
    void rectangleTest(test::Test& test);
    void circleTest(test::Test& test);
    void textureTest(test::Test& test);
    void textureResizeUpTest(test::Test& test);
    void textureResizeDownTest(test::Test& test);
    void textureColorMultiplyTest(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    addTest("basic", [&](test::Test& test) { basicTest(test); });
    addTest("clear", [&](test::Test& test) { clearTest(test); });
    addTest("rectangle", [&](test::Test& test) { rectangleTest(test); });
    addTest("circle", [&](test::Test& test) { circleTest(test); });
    addTest("texture", [&](test::Test& test) { textureTest(test); });
    addTest("texture_resize_up", [&](test::Test& test) { textureResizeUpTest(test); });
    addTest("texture_resize_down", [&](test::Test& test) { textureResizeDownTest(test); });
    addTest("texture_color_multiply", [&](test::Test& test) { textureColorMultiplyTest(test); });
}

void GlvisTestModule::basicTest(test::Test& test) {
    Window window;
    window.create(800, 600, "GLVis Test");
}

void GlvisTestModule::clearTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    window.clear(Color::Red);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(50, 50), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(99, 99), Color::Red, &Color::toString);
}

void GlvisTestModule::rectangleTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(Color::Black);

    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(9, 9), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(10, 10), Color::Black, &Color::toString);
}

void GlvisTestModule::circleTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(Color::Black);

    Circle circle(5.0f);
    circle.setColor(Color::Red);
    window.draw(circle);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(5, 5), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(9, 9), Color::Black, &Color::toString);
}

void GlvisTestModule::textureTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(Color::Black);

    unsigned char data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    Texture tex(data, 2, 2);
    Rectangle rect(2.0f, 2.0f);
    rect.setTexture(&tex);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(1, 2, 3, 4), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(5, 6, 7, 8), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(9, 10, 11, 12), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(13, 14, 15, 16), &Color::toString);
    T_COMPARE(image.getPixel(2, 2), Color::Black, &Color::toString);
}

void GlvisTestModule::textureResizeUpTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    unsigned char data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    Texture tex(data, 2, 2);
    T_COMPARE(tex.getWidth(), 2);
    T_COMPARE(tex.getHeight(), 2);
    tex.resize(4, 4);
    T_COMPARE(tex.getWidth(), 4);
    T_COMPARE(tex.getHeight(), 4);

    Image img = tex.readPixels();
    // Check corners remain the same
    T_COMPARE(img.getPixel(0, 0), Color(1, 2, 3, 4), &Color::toString);
    T_COMPARE(img.getPixel(3, 0), Color(5, 6, 7, 8), &Color::toString);
    T_COMPARE(img.getPixel(0, 3), Color(9, 10, 11, 12), &Color::toString);
    T_COMPARE(img.getPixel(3, 3), Color(13, 14, 15, 16), &Color::toString);
    // Check an interpolated pixel
    T_COMPARE(img.getPixel(1, 1), Color(5, 6, 7, 8), &Color::toString);
}

void GlvisTestModule::textureResizeDownTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    unsigned char data[64];
    for (int i = 0; i < 64; i++) {
        data[i] = ((i / 4) + 1);
    }
    Texture tex(data, 4, 4);
    T_COMPARE(tex.getWidth(), 4);
    T_COMPARE(tex.getHeight(), 4);
    tex.resize(2, 2);
    T_COMPARE(tex.getWidth(), 2);
    T_COMPARE(tex.getHeight(), 2);

    Image img = tex.readPixels();
    // Check corners
    T_COMPARE(img.getPixel(0, 0), Color(1, 1, 1, 1), &Color::toString);
    T_COMPARE(img.getPixel(1, 0), Color(4, 4, 4, 4), &Color::toString);
    T_COMPARE(img.getPixel(0, 1), Color(13, 13, 13, 13), &Color::toString);
    T_COMPARE(img.getPixel(1, 1), Color(16, 16, 16, 16), &Color::toString);
}

void GlvisTestModule::textureColorMultiplyTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(Color::Black);

    unsigned char data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    Texture tex(data, 2, 2);
    Rectangle rect(2.0f, 2.0f);
    rect.setTexture(&tex);
    rect.setColor(Color(64, 128, 192, 32));
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(0, 1, 2, 0), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(1, 3, 5, 1), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(2, 5, 8, 1), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(3, 7, 11, 2), &Color::toString);
    T_COMPARE(image.getPixel(2, 2), Color::Black, &Color::toString);
}

int main() {
    test::TestModule root("GLVis Tests", nullptr);
    root.print_summary_enabled = true;
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();

    // TODO: add linear interpolation test
    // TODO: make RenderTexture tests
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
