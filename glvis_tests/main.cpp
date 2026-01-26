#include <iostream>
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
    void rotateTest(test::Test& test);
    void scaleTest(test::Test& test);
    void textureTest(test::Test& test);
    void textureColorMultiplyTest(test::Test& test);
    void textureResizeTest(test::Test& test);
    void windowResizeTest(test::Test& test);
    void viewPanTest(test::Test& test);
    void viewZoomTest(test::Test& test);
    void viewRotateTest(test::Test& test);
    void vertexArrayTriangleTest(test::Test& test);
    void vertexArrayLineTest(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    window.create(100, 100, "glvis tests");
    auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
    auto rectangle_test = addTest("rectangle", { clear_test }, [&](test::Test& test) { rectangleTest(test); });
    auto circle_test = addTest("circle", { clear_test }, [&](test::Test& test) { circleTest(test); });
    auto move_test = addTest("move", { rectangle_test }, [&](test::Test& test) { moveTest(test); });
    auto rotate_test = addTest("rotate", { rectangle_test }, [&](test::Test& test) { rotateTest(test); });
    auto scale_test = addTest("scale", { rectangle_test }, [&](test::Test& test) { scaleTest(test); });
    auto texture_test = addTest("texture", { rectangle_test }, [&](test::Test& test) { textureTest(test); });
    auto texture_color_multiply_test = addTest("texture_color_multiply", { texture_test }, [&](test::Test& test) { textureColorMultiplyTest(test); });
    auto texture_resize_up_test = addTest("texture_resize", { texture_test }, [&](test::Test& test) { textureResizeTest(test); });
    auto window_resize_test = addTest("window_resize", { rectangle_test }, [&](test::Test& test) { windowResizeTest(test); });
    auto view_pan_test = addTest("view_pan", { rectangle_test }, [&](test::Test& test) { viewPanTest(test); });
    auto view_zoom_test = addTest("view_zoom", { rectangle_test }, [&](test::Test& test) { viewZoomTest(test); });
    auto view_rotate_test = addTest("view_rotate", { rectangle_test }, [&](test::Test& test) { viewRotateTest(test); });
    auto vertex_array_triangle_test = addTest("vertex_array_triangle", { clear_test }, [&](test::Test& test) { vertexArrayTriangleTest(test); });
    auto vertex_array_line_test = addTest("vertex_array_line", { clear_test }, [&](test::Test& test) { vertexArrayLineTest(test); });
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
    T_COMPARE(image.getPixel(window_center), Color::Red, &Color::toString);
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
    T_COMPARE(image.getPixel(new_window_center), Color::Green, &Color::toString);
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
    T_COMPARE(image.getPixel(rect_size - Vector2f(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size), Color::Black, &Color::toString);
}

void GlvisTestModule::circleTest(test::Test& test) {
    const int window_width = 100;
    const int window_height = 100;
    window.setSize(window_width, window_height);
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
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel((int)circle_radius, (int)circle_radius), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel((int)(circle_radius * 2) - 1, (int)(circle_radius * 2) - 1), Color::Black, &Color::toString);
}

void GlvisTestModule::moveTest(test::Test& test) {
   window.setSize(100, 100);
   window.setTitle("move");
   View view;
   view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
   window.setView(view);
   window.clear(Color::Black);

   // render rect
   Rectangle rect(10.0f, 10.0f);
   rect.setColor(Color::Red);
   window.draw(rect);
   window.display();
   Image image = window.readPixels();
   T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(9, 9), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(10, 10), Color::Black, &Color::toString);

   // move rect 10 pixels right and down
   rect.move(10.0f, 10.0f);
   window.clear(Color::Black);
   window.draw(rect);
   window.display();
   image = window.readPixels();

   // check that the rectangle has moved 10 pixels right and down
   T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(9, 9), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(10, 10), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(19, 19), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(20, 20), Color::Black, &Color::toString);
}

void GlvisTestModule::rotateTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("rotate");
    View view;
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check pixels around screen center
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(9, 9), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(10, 10), Color::Black, &Color::toString);

    // rotate rect 45 degrees
    view.setPosition(Vector2f());
    window.setView(view);
    rect.setOrigin(5.0f, 5.0f);
    rect.setRotation(degrees(45.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    
    // check pixels around screen center
    image = window.readPixels();
    const int window_width = window.getWidth();
    const int window_height = window.getHeight();
    Vector2i center = Vector2i(window_width / 2, window_height / 2);
    const int offset = 5;
    // check top left
    T_COMPARE(image.getPixel(center.x - offset,     center.y - offset    ), Color::Black, &Color::toString);
    // check top right
    T_COMPARE(image.getPixel(center.x + offset - 1, center.y - offset    ), Color::Black, &Color::toString);
    // check bottom right
    T_COMPARE(image.getPixel(center.x + offset - 1, center.y + offset - 1), Color::Black, &Color::toString);
    // check bottom left
    T_COMPARE(image.getPixel(center.x - offset,     center.y + offset - 1), Color::Black, &Color::toString);
    // check center
    T_COMPARE(image.getPixel(window_width / 2, window_height / 2), Color::Red, &Color::toString);
}

void GlvisTestModule::scaleTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("scale");
    View view;
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(9, 9), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(10, 10), Color::Black, &Color::toString);

    // scale rect by factor 2
    view.setPosition(Vector2f());
    window.setView(view);
    rect.setOrigin(5.0f, 5.0f);
    rect.setScale(Vector2f(2.0f, 2.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check that the rectangle has scaled by factor 2
    T_WRAP_CONTAINER(checkPixelColor(test, image, 40, 40, 60, 60, Color::Red));

    // check outside
    T_COMPARE(image.getPixel(39, 39), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(60, 60), Color::Black, &Color::toString);
}

void GlvisTestModule::textureTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("texture");
    View view;
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);

    // Create a 2x2 texture
    unsigned char texture_data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    Texture tex(texture_data, 2, 2);
    Rectangle rect(2.0f, 2.0f);
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
    T_COMPARE(image.getPixel(2, 2), Color::Black, &Color::toString);
}

void GlvisTestModule::textureColorMultiplyTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("texture color multiply");
    View view;
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);

    // Render a rectangle with a texture
    unsigned char texture_data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    Texture tex(texture_data, 2, 2);
    Rectangle rect(2.0f, 2.0f);
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
    T_COMPARE(image.getPixel(2, 2), Color::Black, &Color::toString);
}

void GlvisTestModule::textureResizeTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("texture resize interpolation");

    // Test resizing up from 2x1 to 3x1
    unsigned char texture_data[8] = {
        0, 0, 0, 0,
        255, 255, 255, 255
    };
    Texture tex(texture_data, 2, 1);
    T_COMPARE(tex.getWidth(), 2);
    T_COMPARE(tex.getHeight(), 1);
    tex.resize(3, 1);
    T_COMPARE(tex.getWidth(), 3);
    T_COMPARE(tex.getHeight(), 1);
    
    // Check linear interpolation
    Image img = tex.readPixels();
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
    
    // Check linear downsampling
    Image img_down = tex_down.readPixels();
    T_COMPARE(img_down.getPixel(0, 0), Color(32, 32, 32, 32), &Color::toString);
    T_COMPARE(img_down.getPixel(1, 0), Color(223, 223, 223, 223), &Color::toString);
}

void GlvisTestModule::windowResizeTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("window resize");
    View view;
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);

    // Draw a 10x10 red rectangle
    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image initialImage = window.readPixels();

    // Resize to 200x200
    window.setSize(200, 200);
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image resizedImage = window.readPixels();

    // Resize back to 100x100
    window.setSize(100, 100);
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image finalImage = window.readPixels();

    // Compare initial and final images pixel by pixel
    T_WRAP_CONTAINER(compareImages(test, finalImage, initialImage));
}

void GlvisTestModule::viewPanTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("view pan");
    View view;
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(9, 9), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(10, 10), Color::Black, &Color::toString);

    // move View 10 pixels up and left
    Vector2f camera_pos = view.getPosition();
    view.setPosition(view.getPosition() + Vector2f(-10.0f, -10.0f));
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check that the View has panned 10 pixels up and left
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(9, 9), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(10, 10), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(19, 19), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(20, 20), Color::Black, &Color::toString);
}

void GlvisTestModule::viewZoomTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("view zoom");
    View view;
    view.setPosition(Vector2f(5.0f, 5.0f));
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, 45, 45, 55, 55, Color::Red));

    // zoom in by factor 2
    view.setZoom(2.0f);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check that the View has zoomed in
    T_WRAP_CONTAINER(checkPixelColor(test, image, 40, 40, 60, 60, Color::Red));

    // check outside
    T_COMPARE(image.getPixel(39, 39), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(60, 60), Color::Black, &Color::toString);
}

void GlvisTestModule::viewRotateTest(test::Test& test) {
    window.setSize(100, 100);
    window.setTitle("view rotation");
    View view;
    view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    Rectangle rect(10.0f, 10.0f);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(9, 9), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(10, 10), Color::Black, &Color::toString);

    // rotate View 45 degrees
    view.setPosition(Vector2f(5.0f, 5.0f));
    view.setRotation(degrees(45.0f));
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check pixels around screen center
    const int window_width = window.getWidth();
    const int window_height = window.getHeight();
    Vector2i center = Vector2i(window_width / 2, window_height / 2);
    const int offset = 5;
    // check top left
    T_COMPARE(image.getPixel(center.x - offset,     center.y - offset    ), Color::Black, &Color::toString);
    // check top right
    T_COMPARE(image.getPixel(center.x + offset - 1, center.y - offset    ), Color::Black, &Color::toString);
    // check bottom right
    T_COMPARE(image.getPixel(center.x + offset - 1, center.y + offset - 1), Color::Black, &Color::toString);
    // check bottom left
    T_COMPARE(image.getPixel(center.x - offset,     center.y + offset - 1), Color::Black, &Color::toString);
    // check center
    T_COMPARE(image.getPixel(window_width / 2, window_height / 2), Color::Red, &Color::toString);
}

void GlvisTestModule::vertexArrayTriangleTest(test::Test& test) {
   window.setSize(100, 100);
   window.setTitle("vertex array triangle");
   View view;
   view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
   window.setView(view);
   window.clear(Color::Black);

   // Render a triangle
   VertexArray triangle(PrimitiveType::Triangles, 3);
   triangle[0] = Vertex(Vector2f(0, 0), Color::Red, Vector2f(0, 0));
   triangle[1] = Vertex(Vector2f(10, 0), Color::Red, Vector2f(0, 0));
   triangle[2] = Vertex(Vector2f(5, 10), Color::Red, Vector2f(0, 0));
   triangle.syncBuffer();
   window.draw(triangle);
   window.display();

   // Check that the triangle is rendered correctly
   Image image = window.readPixels();
   T_COMPARE(image.getPixel(5, 5), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(2, 2), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(8, 2), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(0, 10), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(10, 10), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(15, 15), Color::Black, &Color::toString);
}

void GlvisTestModule::vertexArrayLineTest(test::Test& test) {
   window.setSize(100, 100);
   window.setTitle("vertex array line");
   View view;
   view.setPosition(Vector2f(window.getWidth() / 2.0f, window.getHeight() / 2.0f));
   window.setView(view);
   window.clear(Color::Black);

   // Render a line
   VertexArray line(PrimitiveType::Lines, 2);
   line[0] = Vertex(Vector2f(10, 50), Color::Red, Vector2f(0, 0));
   line[1] = Vertex(Vector2f(90, 50), Color::Red, Vector2f(0, 0));
   line.syncBuffer();
   window.draw(line);
   window.display();

   // Check line pixels
   Image image = window.readPixels();
   T_COMPARE(image.getPixel(10, 50), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(50, 50), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(89, 50), Color::Red, &Color::toString);

   // Check line edges on the outside
   T_COMPARE(image.getPixel(9, 50), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(90, 50), Color::Black, &Color::toString);

   // Check above and below
   T_COMPARE(image.getPixel(50, 49), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(50, 51), Color::Black, &Color::toString);
}

int main() {
    test::TestModule root("glvis tests", nullptr);
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();
    root.printSummary();

    // TODO: replace hardcoded values in tests
    // TODO: test adding vertices to VertexArray
    // TODO: add usage hints to VertexBuffer
    // TODO: add .editorconfig
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
