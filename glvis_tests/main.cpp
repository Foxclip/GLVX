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
    void rectangleTest(test::Test& test);
    void circleTest(test::Test& test);
    void textureTest(test::Test& test);
    void textureResizeTest(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    addTest("basic", [&](test::Test& test) { basicTest(test); });
    addTest("rectangle", [&](test::Test& test) { rectangleTest(test); });
    addTest("circle", [&](test::Test& test) { circleTest(test); });
    addTest("texture", [&](test::Test& test) { textureTest(test); });
    addTest("texture_resize", [&](test::Test& test) { textureResizeTest(test); });
}

void GlvisTestModule::basicTest(test::Test& test) {
    Window window;
    window.create(800, 600, "GLVis Test");
}

void GlvisTestModule::rectangleTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(ColorRGBA::Black);

    Rectangle rect(10.0f, 10.0f);
    rect.setColor(ColorRGBA::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), ColorRGBA::Red, &ColorRGBA::toString);
    T_COMPARE(image.getPixel(9, 9), ColorRGBA::Red, &ColorRGBA::toString);
    T_COMPARE(image.getPixel(10, 10), ColorRGBA::Black, &ColorRGBA::toString);
}

void GlvisTestModule::circleTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(ColorRGBA::Black);

    Circle circle(5.0f);
    circle.setColor(ColorRGBA::Red);
    window.draw(circle);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), ColorRGBA::Black, &ColorRGBA::toString);
    T_COMPARE(image.getPixel(5, 5), ColorRGBA::Red, &ColorRGBA::toString);
    T_COMPARE(image.getPixel(9, 9), ColorRGBA::Black, &ColorRGBA::toString);
}

void GlvisTestModule::textureTest(test::Test& test) {
    Window window;
    window.create(100, 100, "GLVis Test");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(ColorRGBA::Black);

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
    T_COMPARE(image.getPixel(0, 0), ColorRGBA(1, 2, 3, 4), &ColorRGBA::toString);
    T_COMPARE(image.getPixel(1, 0), ColorRGBA(5, 6, 7, 8), &ColorRGBA::toString);
    T_COMPARE(image.getPixel(0, 1), ColorRGBA(9, 10, 11, 12), &ColorRGBA::toString);
    T_COMPARE(image.getPixel(1, 1), ColorRGBA(13, 14, 15, 16), &ColorRGBA::toString);
    T_COMPARE(image.getPixel(2, 2), ColorRGBA::Black, &ColorRGBA::toString);
}

void GlvisTestModule::textureResizeTest(test::Test& test) {
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
}

int main() {
    test::TestModule root("GLVis Tests", nullptr);
    root.print_summary_enabled = true;
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();

    // TODO: create ImageRGB typedef
    // TODO: add destructor to Texture class
    // TODO: make rendering tests
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
