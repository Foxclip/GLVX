#include <iostream>
#include <cmath>
#include "test_lib/test.h"
#include "glvis/vector.h"
#include "glvis/color.h"
#include "glvis/vertex.h"
#include "glvis/rectangle.h"
#include "glvis/circle.h"
#include "glvis/window.h"
#include "glvis/texture.h"
#include "glvis/angle.h"
#include "glvis/vertex_array.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glvis/utils.h"

using namespace glvis;

class GlvisTestModule : public test::TestModule {
public:
    GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes = { });

private:
    Window window;

    bool checkPixelColor(test::Test& test, const Image& image, int startX, int startY, int endX, int endY, const Color& expectedColor);
    bool compareImages(test::Test& test, const Image& image1, const Image& image2);

    void clearTest(test::Test& test);
    void rectangleTest(test::Test& test);
    void circleTest(test::Test& test);
    void moveTest(test::Test& test);
    void setOriginTest(test::Test& test);
    void rotateTopLeftTest(test::Test& test);
    void rotateCenterTest(test::Test& test);
    void scaleTopLeftTest(test::Test& test);
    void scaleCenterTest(test::Test& test);
    void textureTest(test::Test& test);
    void textureColorMultiplyTest(test::Test& test);
    void textureResizeTest(test::Test& test);
    void windowResizeTest(test::Test& test);
    void viewPanTest(test::Test& test);
    void viewZoomTest(test::Test& test);
    void viewRotateTest(test::Test& test);
    void vertexArrayTriangleTest(test::Test& test);
    void vertexArrayLineTest(test::Test& test);
    void vertexArrayModifyTest(test::Test& test);
    void renderStatesTransformTest(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    window.create(100, 100, "glvis tests");
    auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
    auto rectangle_test = addTest("rectangle", { clear_test }, [&](test::Test& test) { rectangleTest(test); });
    auto circle_test = addTest("circle", { clear_test }, [&](test::Test& test) { circleTest(test); });
    auto move_test = addTest("move", { rectangle_test }, [&](test::Test& test) { moveTest(test); });
    auto set_origin_test = addTest("set_origin", { rectangle_test }, [&](test::Test& test) { setOriginTest(test); });
    auto rotate_rop_left_test = addTest("rotate_top_left", { set_origin_test }, [&](test::Test& test) { rotateTopLeftTest(test); });
    auto rotate_center_test = addTest("rotate_center", { set_origin_test }, [&](test::Test& test) { rotateCenterTest(test); });
    auto scale_top_left_test = addTest("scale_top_left", { set_origin_test }, [&](test::Test& test) { scaleTopLeftTest(test); });
    auto scale_center_test = addTest("scale_center", { set_origin_test }, [&](test::Test& test) { scaleCenterTest(test); });
    auto texture_test = addTest("texture", { rectangle_test }, [&](test::Test& test) { textureTest(test); });
    auto texture_color_multiply_test = addTest("texture_color_multiply", { texture_test }, [&](test::Test& test) { textureColorMultiplyTest(test); });
    auto texture_resize_up_test = addTest("texture_resize", { texture_test }, [&](test::Test& test) { textureResizeTest(test); });
    auto window_resize_test = addTest("window_resize", { rectangle_test }, [&](test::Test& test) { windowResizeTest(test); });
    auto view_pan_test = addTest("view_pan", { rectangle_test }, [&](test::Test& test) { viewPanTest(test); });
    auto view_zoom_test = addTest("view_zoom", { rectangle_test }, [&](test::Test& test) { viewZoomTest(test); });
    auto view_rotate_test = addTest("view_rotate", { rectangle_test }, [&](test::Test& test) { viewRotateTest(test); });
    auto vertex_array_triangle_test = addTest("vertex_array_triangle", { clear_test }, [&](test::Test& test) { vertexArrayTriangleTest(test); });
    auto vertex_array_line_test = addTest("vertex_array_line", { clear_test }, [&](test::Test& test) { vertexArrayLineTest(test); });
    auto vertex_array_modify_test = addTest("vertex_array_modify", { vertex_array_triangle_test }, [&](test::Test& test) { vertexArrayModifyTest(test); });
    auto render_states_transform_test = addTest("render_states_transform", { rectangle_test }, [&](test::Test& test) { renderStatesTransformTest(test); });
}

bool GlvisTestModule::checkPixelColor(test::Test& test, const Image& image, int startX, int startY, int endX, int endY, const Color& expectedColor) {
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            T_CONTAINER(std::format("x: {}, y: {}", x, y));
            if (!T_COMPARE(image.getPixel(x, y), expectedColor, &Color::toString)) {
                return false;
            }
        }
    }
    return true;
}

bool GlvisTestModule::compareImages(test::Test& test, const Image& image1, const Image& image2) {
    int width = image1.getWidth();
    int height = image1.getHeight();
    if (image2.getWidth() != width || image2.getHeight() != height) {
        T_MESSAGE("Images have different dimensions");
        return false;
    }
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            T_CONTAINER(std::format("x: {}, y: {}", x, y));
            if (!T_COMPARE(image1.getPixel(x, y), image2.getPixel(x, y), &Color::toString)) {
                return false;
            }
        }
    }
    return true;
}

void GlvisTestModule::clearTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("clear");

    // Clear the window with red
    window.clear(Color::Red);
    window.display();
    Image image = window.readPixels();
    Vector2f window_center = window.getCenter();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(window_center)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(window_size - Vector2i(1, 1)), Color::Red, &Color::toString);

    // Resize the window
    const Vector2i new_window_size = Vector2i(200, 200);
    window.setSize(new_window_size);

    // Clear the window with green
    window.clear(Color::Green);
    window.display();
    image = window.readPixels();
    Vector2f new_window_center = window.getCenter();
    T_COMPARE(image.getPixel(0, 0), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(new_window_center)), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(new_window_size - Vector2i(1, 1)), Color::Green, &Color::toString);
}

void GlvisTestModule::rectangleTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("rectangle");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // Render a rectangle
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // Check that the rectangle is rendered correctly
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(rect_size) - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(rect_size)), Color::Black, &Color::toString);
}

void GlvisTestModule::circleTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("circle");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // Render a circle
    const float circle_radius = 5.0f;
    Circle circle(circle_radius);
    circle.setColor(Color::Red);
    window.draw(circle);
    window.display();

    // Check that the circle is rendered correctly
    Image image = window.readPixels();
    Vector2i circle_center((int)circle_radius, (int)circle_radius);
    Vector2i circle_bounds_max = circle_center * 2;
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(circle_center), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(circle_bounds_max - Vector2i(1, 1)), Color::Black, &Color::toString);
}

void GlvisTestModule::moveTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("move");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // move rect 10 pixels right and down
    const Vector2f move_offset = Vector2f(10.0f, 10.0f);
    const Vector2f new_pos = rect.getPosition() + move_offset;
    rect.move(move_offset);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has moved 10 pixels right and down
    image = window.readPixels();
    Vector2i new_pos_int = static_cast<Vector2i>(new_pos);
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int + rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int + rect_size_int), Color::Black, &Color::toString);
}

void GlvisTestModule::setOriginTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("set origin");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // render rect with default origin (0,0)
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2f rect_half_size = rect_size / 2.0f;
    const Vector2i rect_half_size_int = static_cast<Vector2i>(rect_half_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, 0, 0, rect_size_int.x, rect_size_int.y, Color::Red));
    T_COMPARE(image.getPixel(rect_size_int + Vector2i(1, 1)), Color::Black, &Color::toString);

    // set origin to (-10,-10)
    rect.setOrigin(-rect_size);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has visually moved right and down
    image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        rect_size_int.x,
        rect_size_int.y,
        rect_size_int.x * 2,
        rect_size_int.y * 2,
        Color::Red
    ));
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int * 2 + Vector2i(1, 1)), Color::Black, &Color::toString);
}

void GlvisTestModule::rotateTopLeftTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("rotate without origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // rotate rect 45 degrees around top-left
    rect.setRotation(degrees(45.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check pixels for rotation around top-left
    image = window.readPixels();
    const Vector2f original_rect_center = rect_half_size;
    const Vector2f new_rect_center = Vector2f(sqrt(2.0f) * original_rect_center.x, 0);
    const Vector2i new_rect_center_screen = static_cast<Vector2i>(window_center + new_rect_center);
    const int pixel_offset = 5;
    const Vector2i top_left     = new_rect_center_screen + Vector2i(-pixel_offset    , -pixel_offset    );
    const Vector2i top_right    = new_rect_center_screen + Vector2i( pixel_offset - 1, -pixel_offset    );
    const Vector2i bottom_right = new_rect_center_screen + Vector2i( pixel_offset - 1,  pixel_offset - 1);
    const Vector2i bottom_left  = new_rect_center_screen + Vector2i(-pixel_offset    ,  pixel_offset - 1);
    T_COMPARE(image.getPixel(top_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(top_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(window_center_int), Color::Red, &Color::toString);
}

void GlvisTestModule::rotateCenterTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("rotate with origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // rotate rect 45 degrees around center
    rect.setOrigin(rect_half_size);
    rect.setRotation(degrees(45.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check pixels around screen center
    image = window.readPixels();
    const int pixel_offset = 5;
    const Vector2i top_left     = window_center_int + Vector2i(-pixel_offset    , -pixel_offset    );
    const Vector2i top_right    = window_center_int + Vector2i( pixel_offset - 1, -pixel_offset    );
    const Vector2i bottom_right = window_center_int + Vector2i( pixel_offset - 1,  pixel_offset - 1);
    const Vector2i bottom_left  = window_center_int + Vector2i(-pixel_offset    ,  pixel_offset - 1);
    T_COMPARE(image.getPixel(top_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(top_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(window_center_int), Color::Red, &Color::toString);
}

void GlvisTestModule::scaleTopLeftTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("scale without origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // scale rect around top-left
    rect.setScale(2.0f, 2.0f);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has scaled
    image = window.readPixels();
    const Vector2i scaled_rect_start = window_center_int;
    const Vector2i scaled_rect_end = window_center_int + rect_size_int * 2;
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        scaled_rect_start.x,
        scaled_rect_start.y,
        scaled_rect_end.x,
        scaled_rect_end.y,
        Color::Red
    ));

    // check outside
    T_COMPARE(image.getPixel(scaled_rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(scaled_rect_end), Color::Black, &Color::toString);
}

void GlvisTestModule::scaleCenterTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("scale with origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // scale rect around center
    rect.setOrigin(rect_half_size);
    rect.setScale(Vector2f(2.0f, 2.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has scaled
    image = window.readPixels();
    const Vector2i half_scaled_size = rect_size_int;
    const Vector2i scaled_rect_start = window_center_int - half_scaled_size;
    const Vector2i scaled_rect_end = window_center_int + half_scaled_size;
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        scaled_rect_start.x,
        scaled_rect_start.y,
        scaled_rect_end.x,
        scaled_rect_end.y,
        Color::Red
    ));

    // check outside
    T_COMPARE(image.getPixel(scaled_rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(scaled_rect_end), Color::Black, &Color::toString);
}

void GlvisTestModule::textureTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("texture");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Create a 2x2 texture
    unsigned char texture_data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    const Vector2i texture_size = Vector2i(2, 2);
    Texture tex(texture_data, texture_size.x, texture_size.y);
    const Vector2f rect_size = Vector2f(
        static_cast<float>(texture_size.x),
        static_cast<float>(texture_size.y)
    );
    Rectangle rect(rect_size);
    rect.setTexture(&tex);
    window.draw(rect);
    window.display();

    // Check that the texture is rendered correctly
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(1, 2, 3, 4), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(5, 6, 7, 8), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(9, 10, 11, 12), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(13, 14, 15, 16), &Color::toString);

    // Check outside of the texture
    T_COMPARE(image.getPixel(texture_size), Color::Black, &Color::toString);
}

void GlvisTestModule::textureColorMultiplyTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("texture color multiply");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Render a rectangle with a texture
    unsigned char texture_data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    const Vector2i texture_size = Vector2i(2, 2);
    Texture tex(texture_data, texture_size.x, texture_size.y);
    const Vector2f rect_size = static_cast<Vector2f>(texture_size);
    Rectangle rect(rect_size);
    rect.setTexture(&tex);
    rect.setColor(Color(64, 128, 192, 32));
    window.draw(rect);
    window.display();

    // Test that the color of the texture is multiplied by the color of the rectangle
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(0, 1, 2, 0), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(1, 3, 5, 1), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(2, 5, 8, 1), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(3, 7, 11, 2), &Color::toString);
    T_COMPARE(image.getPixel(texture_size), Color::Black, &Color::toString);
}

void GlvisTestModule::textureResizeTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("texture resize interpolation");

    // Test resizing up from 2x1 to 3x1
    const Vector2i initial_texture_size = Vector2i(2, 1);
    unsigned char texture_data[8] = {
        0, 0, 0, 0,
        255, 255, 255, 255
    };
    Texture tex(texture_data, initial_texture_size.x, initial_texture_size.y);
    T_COMPARE(tex.getWidth(), initial_texture_size.x);
    T_COMPARE(tex.getHeight(), initial_texture_size.y);
    const Vector2i resized_texture_size = Vector2i(3, 1);
    tex.resize(resized_texture_size.x, resized_texture_size.y);
    T_COMPARE(tex.getWidth(), resized_texture_size.x);
    T_COMPARE(tex.getHeight(), resized_texture_size.y);

    // Check linear interpolation
    Image img = tex.readPixels();
    T_COMPARE(img.getPixel(0, 0), Color(0, 0, 0, 0), &Color::toString);
    T_COMPARE(img.getPixel(1, 0), Color(127, 127, 127, 127), &Color::toString);
    T_COMPARE(img.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);

    // Test resizing down from 3x1 to 2x1
    const Vector2i down_initial_size = Vector2i(3, 1);
    unsigned char data_down[12] = {
        0, 0, 0, 0,
        127, 127, 127, 127,
        255, 255, 255, 255
    };
    Texture tex_down(data_down, down_initial_size.x, down_initial_size.y);
    const Vector2i down_resized_size = Vector2i(2, 1);
    tex_down.resize(down_resized_size.x, down_resized_size.y);

    // Check linear downsampling
    Image img_down = tex_down.readPixels();
    T_COMPARE(img_down.getPixel(0, 0), Color(32, 32, 32, 32), &Color::toString);
    T_COMPARE(img_down.getPixel(1, 0), Color(223, 223, 223, 223), &Color::toString);
}

void GlvisTestModule::windowResizeTest(test::Test& test) {
    const Vector2i initial_window_size = Vector2i(100, 100);
    window.setSize(initial_window_size);
    window.setTitle("window resize");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Draw a 10x10 red rectangle
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image initialImage = window.readPixels();

    // Resize to 200x200
    const Vector2i resized_window_size = Vector2i(200, 200);
    window.setSize(resized_window_size);
    window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image resizedImage = window.readPixels();

    // Resize back to 100x100
    window.setSize(initial_window_size);
    window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image finalImage = window.readPixels();

    // Compare initial and final images pixel by pixel
    T_WRAP_CONTAINER(compareImages(test, finalImage, initialImage));
}

void GlvisTestModule::viewPanTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("view pan");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // move View 10 pixels up and left
    const Vector2f pan_offset = Vector2f(-10.0f, -10.0f);
    view.setPosition(view.getPosition() + pan_offset);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check that the View has panned 10 pixels up and left
    const Vector2i panned_rect_start = Vector2i(10, 10);
    const Vector2i panned_rect_end = panned_rect_start + rect_size_int;
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_start), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_end - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_end), Color::Black, &Color::toString);
}

void GlvisTestModule::viewZoomTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("view zoom");
    View view;
    const Vector2f initial_view_pos = Vector2f(5.0f, 5.0f);
    view.setPosition(initial_view_pos);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    const Vector2i initial_check_start = Vector2i(45, 45);
    const Vector2i initial_check_end = Vector2i(55, 55);
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        initial_check_start.x,
        initial_check_start.y,
        initial_check_end.x,
        initial_check_end.y,
        Color::Red
    ));

    // zoom in by factor 2
    const float zoom_factor = 2.0f;
    view.setZoom(zoom_factor);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check that the View has zoomed in
    const Vector2i zoomed_check_start = Vector2i(40, 40);
    const Vector2i zoomed_check_end = Vector2i(60, 60);
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        zoomed_check_start.x,
        zoomed_check_start.y,
        zoomed_check_end.x,
        zoomed_check_end.y,
        Color::Red
    ));

    // check outside
    T_COMPARE(image.getPixel(zoomed_check_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(zoomed_check_end), Color::Black, &Color::toString);
}

void GlvisTestModule::viewRotateTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("view rotation");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // rotate View 45 degrees
    const Vector2f rotated_view_pos = Vector2f(5.0f, 5.0f);
    view.setPosition(rotated_view_pos);
    const Angle rotation_angle = degrees(45.0f);
    view.setRotation(rotation_angle);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check pixels around screen center
    window_center = window.getCenter();
    const Vector2i center = static_cast<Vector2i>(window_center);
    const int offset = 5;
    const Vector2i top_left     = center + Vector2i(-offset,     -offset    );
    const Vector2i top_right    = center + Vector2i( offset - 1, -offset    );
    const Vector2i bottom_right = center + Vector2i( offset - 1,  offset - 1);
    const Vector2i bottom_left  = center + Vector2i(-offset,      offset - 1);
    T_COMPARE(image.getPixel(top_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(top_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(center), Color::Red, &Color::toString);
}

void GlvisTestModule::vertexArrayTriangleTest(test::Test& test) {
   const Vector2i window_size = Vector2i(100, 100);
   window.setSize(window_size);
   window.setTitle("vertex array triangle");
   View view;
   Vector2f window_center = window.getCenter();
   view.setPosition(window_center);
   window.setView(view);
   window.clear(Color::Black);

   // Render a triangle
   const int triangle_vertex_count = 3;
   VertexArray triangle(PrimitiveType::Triangles, triangle_vertex_count);
   const Vector2f triangle_base_left = Vector2f(0, 0);
   const Vector2f triangle_base_right = Vector2f(10, 0);
   const Vector2f triangle_top = Vector2f(5, 10);
   triangle[0] = Vertex(triangle_base_left, Color::Red, Vector2f(0, 0));
   triangle[1] = Vertex(triangle_base_right, Color::Red, Vector2f(0, 0));
   triangle[2] = Vertex(triangle_top, Color::Red, Vector2f(0, 0));
   window.draw(triangle);
   window.display();

   // Check that the triangle is rendered correctly
   Image image = window.readPixels();
   const Vector2i triangle_center_check = Vector2i(5, 5);
   const Vector2i triangle_left_check = static_cast<Vector2i>(triangle_base_left + Vector2f(2, 2));
   const Vector2i triangle_right_check = static_cast<Vector2i>(triangle_base_right + Vector2f(-2, 2));
   const Vector2i outside_left = static_cast<Vector2i>(triangle_base_left + Vector2f(0, 10));
   const Vector2i outside_right = static_cast<Vector2i>(triangle_base_right);
   const Vector2i outside_far = Vector2i(15, 15);
   T_COMPARE(image.getPixel(triangle_center_check), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(triangle_left_check), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(triangle_right_check), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(outside_left), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(outside_right), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(outside_far), Color::Black, &Color::toString);
}

void GlvisTestModule::vertexArrayLineTest(test::Test& test) {
   const Vector2i window_size = Vector2i(100, 100);
   window.setSize(window_size);
   window.setTitle("vertex array line");
   View view;
   Vector2f window_center = window.getCenter();
   view.setPosition(window_center);
   window.setView(view);
   window.clear(Color::Black);

   // Render a line
   const int line_vertex_count = 2;
   VertexArray line(PrimitiveType::Lines, line_vertex_count);
   const Vector2f line_start = Vector2f(10, 50);
   const Vector2f line_end = Vector2f(90, 50);
   line[0] = Vertex(line_start, Color::Red, Vector2f(0, 0));
   line[1] = Vertex(line_end, Color::Red, Vector2f(0, 0));
   window.draw(line);
   window.display();

   // Check line pixels
   Image image = window.readPixels();
   const Vector2i line_start_int = static_cast<Vector2i>(line_start);
   const Vector2i line_mid = static_cast<Vector2i>((line_start + line_end) / 2.0f);
   const Vector2i line_end_int = static_cast<Vector2i>(line_end);
   T_COMPARE(image.getPixel(line_start_int), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(line_mid), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(line_end_int - Vector2i(1, 0)), Color::Red, &Color::toString);

   // Check line edges on the outside
   T_COMPARE(image.getPixel(line_start_int - Vector2i(1, 0)), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(line_end_int), Color::Black, &Color::toString);

   // Check above and below
   const Vector2i above_line = line_mid - Vector2i(0, 1);
   const Vector2i below_line = line_mid + Vector2i(0, 1);
   T_COMPARE(image.getPixel(above_line), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(below_line), Color::Black, &Color::toString);
}

void GlvisTestModule::vertexArrayModifyTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("vertex array triangle");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Render initial triangle
    const int initial_vertex_count = 3;
    VertexArray vertex_array(PrimitiveType::Triangles, initial_vertex_count);
    const Vector2f triangle_base_left = Vector2f(0, 0);
    const Vector2f triangle_base_right = Vector2f(10, 0);
    const Vector2f triangle_top = Vector2f(5, 10);
    vertex_array[0] = Vertex(triangle_base_left, Color::Red, Vector2f(0, 0));
    vertex_array[1] = Vertex(triangle_base_right, Color::Red, Vector2f(0, 0));
    vertex_array[2] = Vertex(triangle_top, Color::Red, Vector2f(0, 0));
    window.draw(vertex_array);
    window.display();

    // Check initial pixels
    Image initial_image = window.readPixels();
    const Vector2i triangle_center_check = Vector2i(5, 5);
    const Vector2i triangle_left_check = Vector2i(2, 2);
    const Vector2i outside_initial = Vector2i(15, 5);
    T_COMPARE(initial_image.getPixel(triangle_center_check), Color::Red, &Color::toString);
    T_COMPARE(initial_image.getPixel(triangle_left_check), Color::Red, &Color::toString);
    T_COMPARE(initial_image.getPixel(outside_initial), Color::Black, &Color::toString);

    // Add additional vertices to the VertexArray
    const Vector2f second_triangle_base_left = Vector2f(20, 0);
    const Vector2f second_triangle_base_right = Vector2f(30, 0);
    const Vector2f second_triangle_top = Vector2f(25, 10);
    vertex_array.append(Vertex(second_triangle_base_left, Color::Red, Vector2f(0, 0)));
    vertex_array.append(Vertex(second_triangle_base_right, Color::Red, Vector2f(0, 0)));
    vertex_array.append(Vertex(second_triangle_top, Color::Red, Vector2f(0, 0)));
    window.clear(Color::Black);
    window.draw(vertex_array);
    window.display();

    // Check updated pixels
    Image updated_image = window.readPixels();
    const Vector2i second_triangle_center_check = Vector2i(25, 5);
    const Vector2i second_triangle_left_check = Vector2i(22, 2);
    T_COMPARE(initial_image.getPixel(second_triangle_center_check), Color::Black, &Color::toString);
    T_COMPARE(initial_image.getPixel(second_triangle_left_check), Color::Black, &Color::toString);
    T_COMPARE(updated_image.getPixel(triangle_center_check), Color::Red, &Color::toString); // Still red
    T_COMPARE(updated_image.getPixel(second_triangle_center_check), Color::Red, &Color::toString); // New area
    T_COMPARE(updated_image.getPixel(second_triangle_left_check), Color::Red, &Color::toString); // New area
    T_COMPARE(updated_image.getPixel(outside_initial), Color::Black, &Color::toString); // Still black

    // Delete the added vertices (removing the second triangle)
    vertex_array.resize(3);
    window.clear(Color::Black);
    window.draw(vertex_array);
    window.display();

    // Check final image matches initial
    Image final_image = window.readPixels();
    T_WRAP_CONTAINER(compareImages(test, final_image, initial_image));
}

void GlvisTestModule::renderStatesTransformTest(test::Test& test) {
    const Vector2i window_size = Vector2i(100, 100);
    window.setSize(window_size);
    window.setTitle("render states transform");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // Render a rectangle with translation using RenderStates
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2f transform_offset = Vector2f(10.0f, 10.0f);
    const Vector2i transform_offset_int = static_cast<Vector2i>(transform_offset);
    const Color rect_color = Color::Red;
    Rectangle rect(rect_size);
    rect.setColor(rect_color);
    RenderStates states;
    glm::mat4 transform = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(transform_offset.x, transform_offset.y, 0.0f)
    );
    states.transform = from_glmMat4(transform);
    window.draw(rect, states);
    window.display();

    // Check that the rectangle is rendered at translated position
    Image image = window.readPixels();
    const Vector2i rect_start = transform_offset_int;
    const Vector2i rect_end = rect_start + rect_size_int;
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        rect_start.x,
        rect_start.y,
        rect_end.x,
        rect_end.y,
        rect_color
    ));
    T_COMPARE(image.getPixel(rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(rect_end), Color::Black, &Color::toString);
}

int main() {
    test::TestModule root("glvis tests", nullptr);
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();
    root.printSummary();

    // TODO: RenderStates texture test
    // TODO: RenderStates shader test
    // TODO: replace glm objects in renderStatesTransformTest with glvis objects
    // TODO: remove syncBuffer from VertexBuffer and make update method handle partial updates
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
