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
    Window window;

    void clearTest(test::Test& test);
    void rectangleTest(test::Test& test);
    void circleTest(test::Test& test);
    void textureTest(test::Test& test);
    void textureColorMultiplyTest(test::Test& test);
    void textureResizeTest(test::Test& test);
    void windowResizeTest(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    window.create(100, 100, "glvis tests");
    auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
    auto rectangle_test = addTest("rectangle", { clear_test }, [&](test::Test& test) { rectangleTest(test); });
    auto circle_test = addTest("circle", { clear_test }, [&](test::Test& test) { circleTest(test); });
    auto texture_test = addTest("texture", { rectangle_test }, [&](test::Test& test) { textureTest(test); });
    auto texture_color_multiply_test = addTest("texture_color_multiply", { texture_test }, [&](test::Test& test) { textureColorMultiplyTest(test); });
    auto texture_resize_up_test = addTest("texture_resize", { texture_test }, [&](test::Test& test) { textureResizeTest(test); });
    auto window_resize_test = addTest("window_resize", { rectangle_test }, [&](test::Test& test) { windowResizeTest(test); });
}

void GlvisTestModule::clearTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("clear");
    window.clear(Color::Red);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(50, 50), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(99, 99), Color::Red, &Color::toString);
}

void GlvisTestModule::rectangleTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("rectangle");
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
    window.setSize(100, 100);
    window.setTitle("circle");
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
    window.setSize(100, 100);
    window.setTitle("texture");
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

void GlvisTestModule::textureColorMultiplyTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("texture color multiply");
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

void GlvisTestModule::textureResizeTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("texture resize interpolation");
    unsigned char data[8] = {
        0, 0, 0, 0,
        255, 255, 255, 255
    };
    Texture tex(data, 2, 1);
    T_COMPARE(tex.getWidth(), 2);
    T_COMPARE(tex.getHeight(), 1);
    tex.resize(3, 1);
    T_COMPARE(tex.getWidth(), 3);
    T_COMPARE(tex.getHeight(), 1);

    Image img = tex.readPixels();
    // Check linear interpolation
    T_COMPARE(img.getPixel(0, 0), Color(0, 0, 0, 0), &Color::toString);
    T_COMPARE(img.getPixel(1, 0), Color(127, 127, 127, 127), &Color::toString);
    T_COMPARE(img.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);

    // Test resizing down from 3x1 to 2x1
    unsigned char data_down[12] = {
        0, 0, 0, 0,
        127, 127, 127, 127,
        255, 255, 255, 255
    };
    Texture tex_down(data_down, 3, 1);
    tex_down.resize(2, 1);
    Image img_down = tex_down.readPixels();
    // Check linear downsampling
    T_COMPARE(img_down.getPixel(0, 0), Color(32, 32, 32, 32), &Color::toString);
    T_COMPARE(img_down.getPixel(1, 0), Color(223, 223, 223, 223), &Color::toString);
}

void GlvisTestModule::windowResizeTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("window resize");
    Camera camera;
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(Color::Black);

    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image initialImage = window.readPixels();

    // Resize to 200x200
    window.setSize(200, 200);
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image resizedImage = window.readPixels();

    // Resize back to 100x100
    window.setSize(100, 100);
    camera.setPosition(glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setCamera(camera);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image finalImage = window.readPixels();

    // Compare initial and final images pixel by pixel
    bool failed = false;
    for (int x = 0; x < 100; ++x) {
        for (int y = 0; y < 100; ++y) {
            if (!T_COMPARE(finalImage.getPixel(x, y), initialImage.getPixel(x, y), &Color::toString)) {
                failed = true;
                break;
            }
        }
        if (failed) break;
    }
}

int main() {
    test::TestModule root("glvis tests", nullptr);
    root.print_summary_enabled = true;
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();

    // TODO: make RenderTexture tests
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
