#include "glvis_tests/render_texture_tests.h"
#include "glvis/view.h"
#include "glvis/render_texture.h"
#include "glvis_tests/glvis_tests_common.h"

RenderTextureTestsModule::RenderTextureTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto clear_test = addTest("clear", [&](test::Test& test) { clearTest(test); });
}

void RenderTextureTestsModule::clearTest(test::Test& test) {
    RenderTexture renderTexture(WINDOW_SIZE.x, WINDOW_SIZE.y);
    View view;
    renderTexture.setView(view);

    // Clear the render texture with red
    renderTexture.clear(Color::Red);
    Image image = renderTexture.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(WINDOW_SIZE / 2)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(WINDOW_SIZE - Vector2i(1, 1)), Color::Red, &Color::toString);

    // Resize the render texture
    const Vector2i new_size = RESIZED_WINDOW_SIZE;
    renderTexture.resize(new_size.x, new_size.y);

    // Clear the render texture with green
    renderTexture.clear(Color::Green);
    image = renderTexture.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(new_size / 2)), Color::Green, &Color::toString);
    T_COMPARE(image.getPixel(new_size - Vector2i(1, 1)), Color::Green, &Color::toString);
}
