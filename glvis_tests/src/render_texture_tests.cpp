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
    auto pan_test = addTest("pan", [&](test::Test& test) { renderTexturePanTest(test); });
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

void RenderTextureTestsModule::renderTexturePanTest(test::Test& test) {
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
