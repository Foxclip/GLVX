#include "glvis_tests/texture_tests.h"
#include "glvis/texture.h"
#include "glvis/vertex_array.h"

TextureTestsModule::TextureTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto texture_full_alpha_test = addTest("texture", [&](test::Test& test) { textureTest(test); });
    auto texture_alpha_test = addTest("texture_alpha", [&](test::Test& test) { textureAlphaTest(test); });
    auto texture_color_multiply_test = addTest("texture_color_multiply", { texture_full_alpha_test }, [&](test::Test& test) { textureColorMultiplyTest(test); });
    auto texture_resize_up_test = addTest("texture_resize", { texture_full_alpha_test }, [&](test::Test& test) { textureResizeTest(test); });
    auto texture_interpolation_test = addTest("texture_interpolation", { texture_full_alpha_test }, [&](test::Test& test) { textureInterpolationTest(test); });
    auto texture_rendering_interpolation_test = addTest("texture_rendering_interpolation", { texture_full_alpha_test }, [&](test::Test& test) { textureRenderingInterpolationTest(test); });
    auto texture_wrapping_test = addTest("texture_wrapping", { texture_full_alpha_test }, [&](test::Test& test) { textureWrappingTest(test); });
}

std::string TextureTestsModule::interpToString(InterpolationType t) {
    return t == InterpolationType::Nearest ? "Nearest" : "Linear";
}

std::string TextureTestsModule::wrapToString(WrappingType t) {
    switch (t) {
        case WrappingType::ClampToEdge: return "ClampToEdge";
        case WrappingType::Repeat: return "Repeat";
        case WrappingType::MirroredRepeat: return "MirroredRepeat";
        case WrappingType::ClampToBorder: return "ClampToBorder";
    }
    return "Unknown";
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

    // Test resizing up from 2x1 to 3x1 with linear interpolation
    const Vector2i initial_texture_size = Vector2i(2, 1);
    unsigned char texture_data[8] = {
        0, 0, 0, 0,
        255, 255, 255, 255
    };
    Texture tex(texture_data, initial_texture_size.x, initial_texture_size.y, 4, InterpolationType::Linear);
    T_COMPARE(tex.getWidth(), initial_texture_size.x);
    T_COMPARE(tex.getHeight(), initial_texture_size.y);
    T_COMPARE(tex.getInterpolation(), InterpolationType::Linear, interpToString);
    const Vector2i resized_texture_size = Vector2i(3, 1);
    tex.resize(resized_texture_size.x, resized_texture_size.y);
    T_COMPARE(tex.getWidth(), resized_texture_size.x);
    T_COMPARE(tex.getHeight(), resized_texture_size.y);

    // Check linear interpolation
    Image img = tex.readPixels();
    T_COMPARE(img.getPixel(0, 0), Color(0, 0, 0, 0), &Color::toString);
    T_COMPARE(img.getPixel(1, 0), Color(127, 127, 127, 127), &Color::toString);
    T_COMPARE(img.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);

    // Test resizing down from 3x1 to 2x1 with linear interpolation
    const Vector2i down_initial_size = Vector2i(3, 1);
    unsigned char data_down[12] = {
        0, 0, 0, 0,
        127, 127, 127, 127,
        255, 255, 255, 255
    };
    Texture tex_down(data_down, down_initial_size.x, down_initial_size.y, 4, InterpolationType::Linear);
    const Vector2i down_resized_size = Vector2i(2, 1);
    tex_down.resize(down_resized_size.x, down_resized_size.y);

    // Check linear downsampling
    Image img_down = tex_down.readPixels();
    T_COMPARE(img_down.getPixel(0, 0), Color(32, 32, 32, 32), &Color::toString);
    T_COMPARE(img_down.getPixel(1, 0), Color(223, 223, 223, 223), &Color::toString);
}

void TextureTestsModule::textureInterpolationTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("texture interpolation");

    // Test Nearest interpolation: resize 2x1 to 4x1, pixels should not blend
    unsigned char nearest_data[8] = {
        0, 0, 0, 255,
        255, 255, 255, 255
    };
    Texture texNearest(nearest_data, 2, 1, 4, InterpolationType::Nearest);
    T_COMPARE(texNearest.getInterpolation(), InterpolationType::Nearest, interpToString);
    texNearest.resize(4, 1);
    Image imgNearest = texNearest.readPixels();
    T_COMPARE(imgNearest.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imgNearest.getPixel(1, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imgNearest.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imgNearest.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);

    // Test Linear interpolation: same resize should produce blended pixels
    Texture texLinear(nearest_data, 2, 1, 4, InterpolationType::Linear);
    T_COMPARE(texLinear.getInterpolation(), InterpolationType::Linear, interpToString);
    texLinear.resize(4, 1);
    Image imgLinear = texLinear.readPixels();
    T_COMPARE(imgLinear.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imgLinear.getPixel(1, 0), Color(64, 64, 64, 255), &Color::toString);
    T_COMPARE(imgLinear.getPixel(2, 0), Color(191, 191, 191, 255), &Color::toString);
    T_COMPARE(imgLinear.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);

    // Test runtime setInterpolation
    unsigned char switch_data[8] = {
        0, 0, 0, 255,
        255, 255, 255, 255
    };
    Texture texSwitch(switch_data, 2, 1, 4, InterpolationType::Nearest);
    T_COMPARE(texSwitch.getInterpolation(), InterpolationType::Nearest, interpToString);
    texSwitch.setInterpolation(InterpolationType::Linear);
    T_COMPARE(texSwitch.getInterpolation(), InterpolationType::Linear, interpToString);
    texSwitch.resize(4, 1);
    Image imgSwitch = texSwitch.readPixels();
    T_COMPARE(imgSwitch.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imgSwitch.getPixel(1, 0), Color(64, 64, 64, 255), &Color::toString);
    T_COMPARE(imgSwitch.getPixel(2, 0), Color(191, 191, 191, 255), &Color::toString);
    T_COMPARE(imgSwitch.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);

    // Switch back to Nearest and verify
    unsigned char switch_back_data[8] = {
        0, 0, 0, 255,
        255, 255, 255, 255
    };
    Texture texSwitchBack(switch_back_data, 2, 1, 4, InterpolationType::Linear);
    texSwitchBack.setInterpolation(InterpolationType::Nearest);
    T_COMPARE(texSwitchBack.getInterpolation(), InterpolationType::Nearest, interpToString);
    texSwitchBack.resize(4, 1);
    Image imgSwitchBack = texSwitchBack.readPixels();
    T_COMPARE(imgSwitchBack.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imgSwitchBack.getPixel(1, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imgSwitchBack.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imgSwitchBack.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);
}

void TextureTestsModule::textureRenderingInterpolationTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("texture rendering interpolation");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    unsigned char texture_data[8] = {
        0, 0, 0, 255,
        255, 255, 255, 255
    };
    Rectangle rect(Vector2f(2, 1));
    Texture texture(texture_data, 2, 1, 4);
    rect.setTexture(&texture);

    // draw 2x1 texture without any transformations
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image image_orig = window.readPixels();
    T_COMPARE(image_orig.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(image_orig.getPixel(1, 0), Color(255, 255, 255, 255), &Color::toString);

    // set interpolation to nearest and resize rect
    texture.setInterpolation(InterpolationType::Nearest);
    rect.setSize(Vector2f(4, 1));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image image_nearest = window.readPixels();
    T_COMPARE(image_nearest.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(image_nearest.getPixel(1, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(image_nearest.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(image_nearest.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);

    // set interpolation to linear
    texture.setInterpolation(InterpolationType::Linear);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    Image image_linear = window.readPixels();
    T_COMPARE(image_linear.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(image_linear.getPixel(1, 0), Color(64, 64, 64, 255), &Color::toString);
    T_COMPARE(image_linear.getPixel(2, 0), Color(191, 191, 191, 255), &Color::toString);
    T_COMPARE(image_linear.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);
}

void TextureTestsModule::textureWrappingTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("texture wrapping");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);

    unsigned char texture_data[8] = {
        0, 0, 0, 255,
        255, 255, 255, 255
    };
    Texture texture(texture_data, 2, 1, 4);

    VertexArray va(PrimitiveType::TriangleStrip, 4);
    va[0].position = Vector2f(0.0f, 0.0f); va[0].texCoords = Vector2f(0, 0);
    va[1].position = Vector2f(4.0f, 0.0f); va[1].texCoords = Vector2f(2, 0);
    va[2].position = Vector2f(0.0f, 1.0f); va[2].texCoords = Vector2f(0, 1);
    va[3].position = Vector2f(4.0f, 1.0f); va[3].texCoords = Vector2f(2, 1);
    va[0].color = va[1].color = va[2].color = va[3].color = Color::White;
    va.setTexture(&texture);

    // ClampToEdge: UV beyond [0,1] clamps to edge (white)
    texture.setWrapping(WrappingType::ClampToEdge);
    T_COMPARE(texture.getWrapping(), WrappingType::ClampToEdge, wrapToString);
    window.clear(Color::Black);
    window.draw(va);
    window.display();
    Image imageClampToEdge = window.readPixels();
    T_COMPARE(imageClampToEdge.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imageClampToEdge.getPixel(1, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imageClampToEdge.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imageClampToEdge.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);

    // Repeat: UV beyond [0,1] repeats the pattern
    texture.setWrapping(WrappingType::Repeat);
    T_COMPARE(texture.getWrapping(), WrappingType::Repeat, wrapToString);
    window.clear(Color::Black);
    window.draw(va);
    window.display();
    Image imageRepeat = window.readPixels();
    T_COMPARE(imageRepeat.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imageRepeat.getPixel(1, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imageRepeat.getPixel(2, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imageRepeat.getPixel(3, 0), Color(255, 255, 255, 255), &Color::toString);

    // MirroredRepeat: UV beyond [0,1] mirrors the pattern
    texture.setWrapping(WrappingType::MirroredRepeat);
    T_COMPARE(texture.getWrapping(), WrappingType::MirroredRepeat, wrapToString);
    window.clear(Color::Black);
    window.draw(va);
    window.display();
    Image imageMirroredRepeat = window.readPixels();
    T_COMPARE(imageMirroredRepeat.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imageMirroredRepeat.getPixel(1, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imageMirroredRepeat.getPixel(2, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imageMirroredRepeat.getPixel(3, 0), Color(0, 0, 0, 255), &Color::toString);

    // ClampToBorder: UV beyond [0,1] uses border color (black)
    texture.setWrapping(WrappingType::ClampToBorder);
    T_COMPARE(texture.getWrapping(), WrappingType::ClampToBorder, wrapToString);
    window.clear(Color::Black);
    window.draw(va);
    window.display();
    Image imageClampToBorder = window.readPixels();
    T_COMPARE(imageClampToBorder.getPixel(0, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imageClampToBorder.getPixel(1, 0), Color(255, 255, 255, 255), &Color::toString);
    T_COMPARE(imageClampToBorder.getPixel(2, 0), Color(0, 0, 0, 255), &Color::toString);
    T_COMPARE(imageClampToBorder.getPixel(3, 0), Color(0, 0, 0, 255), &Color::toString);
}
