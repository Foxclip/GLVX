#include "glvis_tests/texture_tests.h"
#include "glvis/texture.h"

TextureTestsModule::TextureTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto texture_full_alpha_test = addTest("texture", [&](test::Test& test) { textureTest(test); });
    auto texture_alpha_test = addTest("texture_alpha", [&](test::Test& test) { textureAlphaTest(test); });
    auto texture_color_multiply_test = addTest("texture_color_multiply", { texture_full_alpha_test }, [&](test::Test& test) { textureColorMultiplyTest(test); });
    auto texture_resize_up_test = addTest("texture_resize", { texture_full_alpha_test }, [&](test::Test& test) { textureResizeTest(test); });
}

void TextureTestsModule::textureTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("texture");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Create a 2x2 texture with full alpha
    unsigned char texture_data[16] = {
        1, 2, 3, 255,
        5, 6, 7, 255,
        9, 10, 11, 255,
        13, 14, 15, 255
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

    // With full alpha blending against black: Result = Src * 1 + Dst * 0 = Src
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(1, 2, 3, 255), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(5, 6, 7, 255), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(9, 10, 11, 255), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(13, 14, 15, 255), &Color::toString);

    // Check outside of the texture
    T_COMPARE(image.getPixel(texture_size), Color::Black, &Color::toString);
}

void TextureTestsModule::textureAlphaTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("texture alpha");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Create a 2x2 texture with varying alpha
    // Per-channel formula: src_channel * (alpha/255) + 0 * (1 - alpha/255)
    unsigned char texture_data[16] = {
        255, 0, 0, 255,
        0, 255, 0, 128,
        0, 0, 255, 64,
        255, 255, 0, 32
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

    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(255, 0, 0, 255), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(0, 128, 0, 191), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(0, 0, 64, 207), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(32, 32, 0, 227), &Color::toString);

    // Check outside of the texture
    T_COMPARE(image.getPixel(texture_size), Color::Black, &Color::toString);
}

void TextureTestsModule::textureColorMultiplyTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("texture color multiply");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Render a rectangle with a texture
    unsigned char texture_data[16] = {
        1, 2, 3, 255,
        4, 5, 6, 255,
        7, 8, 9, 255,
        10, 11, 12, 255
    };
    const Vector2i texture_size = Vector2i(2, 2);
    Texture tex(texture_data, texture_size.x, texture_size.y);
    const Vector2f rect_size = static_cast<Vector2f>(texture_size);
    Rectangle rect(rect_size);
    rect.setTexture(&tex);
    rect.setColor(Color(128, 128, 128, 255));
    window.draw(rect);
    window.display();

    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(0, 1, 1, 255), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(2, 2, 3, 255), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(3, 4, 4, 255), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(5, 5, 6, 255), &Color::toString);
    T_COMPARE(image.getPixel(texture_size), Color::Black, &Color::toString);
}

void TextureTestsModule::textureResizeTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
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
