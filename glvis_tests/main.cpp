#include <iostream>
#include "test_lib/test.h"
#include "glvis/vector.h"
#include "glvis/color.h"
#include "glvis/vertex.h"
#include "glvis/rectangle.h"
#include "glvis/window.h"

using namespace glvis;

class GlvisTestModule : public test::TestModule {
public:
    GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes = { });

private:
    void basicTest(test::Test& test);
    void rectangleTest(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    addTest("basic", [&](test::Test& test) { basicTest(test); });
    addTest("rectangle", [&](test::Test& test) { rectangleTest(test); });
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
    Image<ColorRGB> image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), ColorRGB::Red, &ColorRGB::toString);
    T_COMPARE(image.getPixel(9, 9), ColorRGB::Red, &ColorRGB::toString);
    T_COMPARE(image.getPixel(10, 10), ColorRGB::Black, &ColorRGB::toString);
}

int main() {
    test::TestModule root("GLVis Tests", nullptr);
    root.print_summary_enabled = true;
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();

    // TODO: make rendering tests
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
