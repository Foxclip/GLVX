#include "glvis_tests/render_texture_tests.h"
#include "glvis/rectangle.h"
#include "glvis/view.h"
#include "glvis/render_texture.h"
#include "glvis_tests/glvis_tests_common.h"

RenderTextureTestsModule::RenderTextureTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
    auto draw_rect_test = addTest("draw_rectangle", { clear_test }, [&](test::Test& test) { drawRectTest(test); });
    auto draw_rect_full_test = addTest("draw_rectangle_full", { draw_rect_test }, [&](test::Test& test) { drawRectFullTest(test); });
    auto pan_test = addTest("pan", { draw_rect_full_test }, [&](test::Test& test) { panTest(test); });
    auto transparent_rect_test = addTest("transparent_rectangle", { draw_rect_full_test }, [&](test::Test& test) { transparentRectangleTest(test); });
}

void RenderTextureTestsModule::clearTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View view;
    render_texture.setView(view);

    // Clear the render texture with red
    render_texture.clear(Color::Red);
    Image image = render_texture.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(WINDOW_SIZE / 2)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(WINDOW_SIZE - Vector2i(1, 1)), Color::Red, &Color::toString);

    // Resize the render texture
    const Vector2i new_size = RESIZED_WINDOW_SIZE;
    render_texture.resize(new_size.x, new_size.y);

    // Clear the render texture with green
    render_texture.clear(Color::Green);
    image = render_texture.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(new_size / 2)), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(new_size - Vector2i(1, 1)), Color::Green, &Color::toString);
}

void RenderTextureTestsModule::drawRectTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View rt_view;
    rt_view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    render_texture.setView(rt_view);
    Color render_texture_background_color = Color(0, 0, 0, 0);
    render_texture.clear(render_texture_background_color);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    render_texture.draw(rect);

    Image image = render_texture.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2i rect_start = Vector2i(0, 0);
    const Vector2i rect_end = rect_start + rect_size_int;
    T_WRAP_CONTAINER(checkPixelColor(test, image, rect_start, rect_end, Color::Red));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y),
        render_texture_background_color
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y),
        render_texture_background_color
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        rect_size_int, WINDOW_SIZE,
        render_texture_background_color
    ));
}

void RenderTextureTestsModule::drawRectFullTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View rt_view;
    rt_view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    render_texture.setView(rt_view);
    render_texture.clear(Color(0, 0, 0, 0));

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    render_texture.draw(rect);

    window.setSize(WINDOW_SIZE);
    window.setTitle("draw rect full");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Rectangle screen_rect(static_cast<Vector2f>(WINDOW_SIZE));
    screen_rect.setTexture(&render_texture);
    window.draw(screen_rect);
    window.display();

    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2i rect_start = Vector2i(0, 0);
    const Vector2i rect_end = rect_start + rect_size_int;
    T_WRAP_CONTAINER(checkPixelColor(test, image, rect_start, rect_end, Color::Red));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        rect_size_int, WINDOW_SIZE,
        Color::Black
    ));
}

void RenderTextureTestsModule::panTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View view;
    Vector2f texture_center = static_cast<Vector2f>(WINDOW_SIZE) / 2.0f;
    view.setPosition(texture_center);
    render_texture.setView(view);
    render_texture.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    render_texture.draw(rect);
    Image image = render_texture.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // move View 10 pixels up and left
    const Vector2f pan_offset = Vector2f(-10.0f, -10.0f);
    view.setPosition(view.getPosition() + pan_offset);
    render_texture.setView(view);
    render_texture.clear(Color::Black);
    render_texture.draw(rect);
    image = render_texture.readPixels();

    // check that the View has panned 10 pixels up and left
    const Vector2i panned_rect_start = Vector2i(10, 10);
    const Vector2i panned_rect_end = panned_rect_start + rect_size_int;
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_start), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_end - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_end), Color::Black, &Color::toString);
}

void RenderTextureTestsModule::transparentRectangleTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View rt_view;
    rt_view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    render_texture.setView(rt_view);
    render_texture.clear(Color(0, 0, 0, 0));

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle white_rect(rect_size);
    white_rect.setColor(Color(255, 255, 255, 128));
    render_texture.draw(white_rect);

    window.setSize(WINDOW_SIZE);
    window.setTitle("transparent rectangle");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Rectangle screen_rect(static_cast<Vector2f>(WINDOW_SIZE));
    screen_rect.setTexture(&render_texture);
    window.draw(screen_rect);
    window.display();

    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2i rect_start = Vector2i(0, 0);
    const Vector2i rect_end = rect_start + rect_size_int;
    T_WRAP_CONTAINER(checkPixelColor(test, image, rect_start, rect_end, Color(32, 32, 32, 207)));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        rect_size_int, WINDOW_SIZE,
        Color::Black
    ));
}
