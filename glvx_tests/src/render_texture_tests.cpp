#include "glvx_tests/render_texture_tests.h"
#include "glvx/rectangle.h"
#include "glvx/view.h"
#include "glvx/render_texture.h"
#include "glvx_tests/glvx_tests_common.h"

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
    auto copy_from_test = addTest("copy_from", { clear_test }, [&](test::Test& test) { copyFromTest(test); });
    auto copy_from_window_test = addTest("copy_from_window", { clear_test }, [&](test::Test& test) { copyFromWindowTest(test); });
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
    View rt_view;
    rt_view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    render_texture.setView(rt_view);
    render_texture.clear(Color(0, 0, 0, 0));

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    render_texture.draw(rect);

    window.setSize(WINDOW_SIZE);
    window.setTitle("pan");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    Rectangle screen_rect(static_cast<Vector2f>(WINDOW_SIZE));
    screen_rect.setTexture(&render_texture);
    window.draw(screen_rect);
    window.display();

    // check rectangle is rendered correctly in the left upper corner
    Image image_orig = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2i rect_start = Vector2i(0, 0);
    const Vector2i rect_end = rect_start + rect_size_int;
    T_WRAP_CONTAINER(checkPixelColor(test, image_orig, rect_start, rect_end, Color::Red));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_orig,
        Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_orig,
        Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_orig,
        rect_size_int, WINDOW_SIZE,
        Color::Black
    ));

    // move View 10 pixels up and left
    const Vector2f pan_offset = Vector2f(-10.0f, -10.0f);
    const Vector2i pan_offset_int = static_cast<Vector2i>(pan_offset);
    rt_view.setPosition(rt_view.getPosition() + pan_offset);
    render_texture.setView(rt_view);
    render_texture.clear(Color::Black);
    render_texture.draw(rect);

    window.draw(screen_rect);
    window.display();

    // check that the View has panned 10 pixels up and left
    Image image_panned = window.readPixels();
    const Vector2i panned_rect_start = rect_size_int;
    const Vector2i panned_rect_end = panned_rect_start + rect_size_int;
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_panned,
        Vector2i(0, 0),
        Vector2i(WINDOW_SIZE.x, -pan_offset_int.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_panned,
        Vector2i(0, -pan_offset_int.y),
        Vector2i(-pan_offset_int.x, -pan_offset_int.y + rect_size_int.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_panned,
        -pan_offset_int,
        -pan_offset_int + rect_size_int,
        Color::Red
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_panned,
        Vector2i(-pan_offset_int.x + rect_size_int.x, -pan_offset_int.y),
        Vector2i(WINDOW_SIZE.x, -pan_offset_int.y + rect_size_int.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_panned,
        Vector2i(0, -pan_offset_int.y + rect_size_int.y),
        Vector2i(WINDOW_SIZE.x, WINDOW_SIZE.y),
        Color::Black
    ));
}

void RenderTextureTestsModule::transparentRectangleTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View rt_view;
    rt_view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    render_texture.setView(rt_view);
    Color render_texture_background = Color(0, 0, 0, 0);
    render_texture.clear(render_texture_background);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle white_rect(rect_size);
    white_rect.setColor(Color(255, 0, 0, 128));
    render_texture.draw(white_rect);

    Image image_rt = render_texture.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2i rect_start = Vector2i(0, 0);
    const Vector2i rect_end = rect_start + rect_size_int;
    // The render texture stores the premultiplied color (128, 0, 0, 128);
    // readPixels converts it back to straight alpha.
    T_WRAP_CONTAINER(checkPixelColor(test, image_rt, rect_start, rect_end, Color(255, 0, 0, 128)));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_rt,
        Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y),
        render_texture_background
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_rt,
        Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y),
        render_texture_background
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_rt,
        rect_size_int, WINDOW_SIZE,
        render_texture_background
    ));

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

    Image image_window = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image_window, rect_start, rect_end, Color(128, 0, 0, 255)));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_window,
        Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_window,
        Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y),
        Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image_window,
        rect_size_int, WINDOW_SIZE,
        Color::Black
    ));
}

void RenderTextureTestsModule::copyFromTest(test::Test& test) {
    // Source: the left half is red, the right half is green
    RenderTexture source(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View source_view;
    source_view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    source.setView(source_view);
    source.clear(Color::Black);

    const float half_width = static_cast<float>(WINDOW_SIZE.x / 2);
    const float full_height = static_cast<float>(WINDOW_SIZE.y);
    Rectangle left_half(half_width, full_height);
    left_half.setColor(Color::Red);
    source.draw(left_half);
    Rectangle right_half(half_width, full_height);
    right_half.setColor(Color::Green);
    right_half.setPosition(half_width, 0.0f);
    source.draw(right_half);

    // The destination is a quarter of the source size
    const Vector2i destination_size = WINDOW_SIZE / 4;
    RenderTexture destination(destination_size.x, destination_size.y);
    destination.copyFrom(source);
    Image image = destination.readPixels();
    T_COMPARE(image.getWidth(), destination_size.x);
    T_COMPARE(image.getHeight(), destination_size.y);

    // Skip the center boundary where linear filtering can blend the colors
    const int boundary_x = destination_size.x / 2;
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(0, 0), Vector2i(boundary_x - 3, destination_size.y),
        Color::Red
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(boundary_x + 3, 0), destination_size,
        Color::Green
    ));
}

void RenderTextureTestsModule::copyFromWindowTest(test::Test& test) {
    // The source is the window itself; depending on the window's state it is
    // either the default framebuffer or the window's offscreen render target.
    window.setSize(WINDOW_SIZE);
    View view;
    view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    window.setView(view);
    window.clear(Color::Black);

    // The top half of the window is red, the bottom half is green; the
    // vertical asymmetry verifies that the copy is not flipped
    const float full_width = static_cast<float>(WINDOW_SIZE.x);
    const float half_height = static_cast<float>(WINDOW_SIZE.y / 2);
    Rectangle top_half(full_width, half_height);
    top_half.setColor(Color::Red);
    window.draw(top_half);
    Rectangle bottom_half(full_width, half_height);
    bottom_half.setColor(Color::Green);
    bottom_half.setPosition(0.0f, half_height);
    window.draw(bottom_half);

    // The destination is a quarter of the window size
    const Vector2i destination_size = WINDOW_SIZE / 4;
    RenderTexture destination(destination_size.x, destination_size.y);
    destination.copyFrom(window);
    Image image = destination.readPixels();
    T_COMPARE(image.getWidth(), destination_size.x);
    T_COMPARE(image.getHeight(), destination_size.y);

    // Skip the center boundary where linear filtering can blend the colors
    const int boundary_y = destination_size.y / 2;
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(0, 0), Vector2i(destination_size.x, boundary_y - 3),
        Color::Red
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image,
        Vector2i(0, boundary_y + 3), destination_size,
        Color::Green
    ));
}
