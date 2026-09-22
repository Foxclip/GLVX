#include "glvx_tests/blend_mode_tests.h"

#include "glvx/blend_mode.h"
#include "glvx/rectangle.h"
#include "glvx/render_states.h"
#include <cmath>

inline const Color BLEND_SRC_COLOR(200, 100, 100, 128);
inline const Color BLEND_BG_COLOR(100, 150, 128);

BlendModeTestsModule::BlendModeTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    addTest("blend_mode_default", [&](test::Test& test) { blendModeDefaultTest(test); });
    addTest("blend_mode_alpha", [&](test::Test& test) { blendModeAlphaTest(test); });
    addTest("blend_mode_add", [&](test::Test& test) { blendModeAddTest(test); });
    addTest("blend_mode_multiply", [&](test::Test& test) { blendModeMultiplyTest(test); });
    addTest("blend_mode_none", [&](test::Test& test) { blendModeNoneTest(test); });
    addTest("blend_mode_operator_equal", [&](test::Test& test) { blendModeOperatorEqualTest(test); });
    addTest("blend_mode_custom", [&](test::Test& test) { blendModeCustomTest(test); });
}

void BlendModeTestsModule::blendModeDefaultTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("blend_mode_default");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Color rect_color = Color(64, 64, 64, 64);
    Rectangle rect(rect_size);
    rect.setColor(rect_color);

    RenderStates states;
    const Color bg_color = Color(32, 32, 32, 32);
    window.clear(bg_color);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    // The pipeline stores premultiplied color and composites with "Over"
    // ((One, OneMinusSrcAlpha)); readPixels converts back to straight alpha.
    // The expected result is therefore the straight "Over" composite of the
    // rectangle color over the clear color.
    float aSrc = rect_color.a / 255.0f;
    float aDst = bg_color.a / 255.0f;
    float cSrc = 64.0f;
    float cDst = 32.0f;
    float aOut = aSrc + aDst * (1.0f - aSrc);
    float cOut = (cSrc * aSrc + cDst * aDst * (1.0f - aSrc)) / aOut;
    int expected_color = static_cast<int>(std::round(cOut));
    int expected_alpha = static_cast<int>(std::round(aOut * 255.0f));
    Color blended_rect_color = Color(expected_color, expected_color, expected_color, expected_alpha);
    Vector2i rect_bottom_right = rect_size_int - Vector2i(1, 1);
    Vector2i rect_bottom_right_outside = rect_size_int;
    T_COMPARE(image.getPixel(0, 0), blended_rect_color, &Color::toString);
    T_COMPARE(image.getPixel(rect_bottom_right), blended_rect_color, &Color::toString);
    T_COMPARE(image.getPixel(rect_bottom_right_outside), bg_color, &Color::toString);
}

void BlendModeTestsModule::blendModeAlphaTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("blend_mode_alpha");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(BLEND_SRC_COLOR);

    RenderStates states;
    states.blend_mode = BlendAlpha;
    window.clear(BLEND_BG_COLOR);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    // "Over" composite of (200, 100, 100, 128) on (100, 150, 128, 255)
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color(150, 125, 114, 255)));
}

void BlendModeTestsModule::blendModeAddTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("blend_mode_add");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(BLEND_SRC_COLOR);

    RenderStates states;
    states.blend_mode = BlendAdd;
    window.clear(BLEND_BG_COLOR);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color(200, 200, 178, 255)));
}

void BlendModeTestsModule::blendModeMultiplyTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("blend_mode_multiply");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(BLEND_SRC_COLOR);

    RenderStates states;
    states.blend_mode = BlendMultiply;
    window.clear(BLEND_BG_COLOR);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    float aSrc = BLEND_SRC_COLOR.a / 255.0f;
    float aDst = BLEND_BG_COLOR.a / 255.0f;
    int stored_r = static_cast<int>(BLEND_BG_COLOR.r * BLEND_SRC_COLOR.r * aSrc * aDst / 255.0f);
    int stored_g = static_cast<int>(BLEND_BG_COLOR.g * BLEND_SRC_COLOR.g * aSrc * aDst / 255.0f);
    int stored_b = static_cast<int>(BLEND_BG_COLOR.b * BLEND_SRC_COLOR.b * aSrc * aDst / 255.0f);
    int stored_a = static_cast<int>(aSrc * aDst * 255.0f);
    int expected_r = static_cast<int>(std::round(stored_r * 255.0f / stored_a));
    int expected_g = static_cast<int>(std::round(stored_g * 255.0f / stored_a));
    int expected_b = static_cast<int>(std::round(stored_b * 255.0f / stored_a));
    Color blended_rect_color = Color(expected_r, expected_g, expected_b, stored_a);
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, blended_rect_color));
}

void BlendModeTestsModule::blendModeNoneTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("blend_mode_none");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(BLEND_SRC_COLOR);

    RenderStates states;
    states.blend_mode = BlendNone;
    window.clear(BLEND_BG_COLOR);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    // No blending: the stored (premultiplied) source color (100, 50, 50, 128)
    // is read back unpremultiplied.
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color(199, 100, 100, 128)));
}

void BlendModeTestsModule::blendModeOperatorEqualTest(test::Test& test) {
    BlendMode bm1;
    BlendMode bm2;
    T_CHECK(bm1 == bm2);
    T_CHECK(!(bm1 != bm2));

    BlendMode bm3;
    bm3.colorSrcFactor = BlendMode::Factor::Zero;
    T_CHECK(!(bm1 == bm3));
    T_CHECK(bm1 != bm3);

    T_CHECK(BlendDefault == BlendMode());
    // BlendAlpha is the premultiplied "Over" composite, identical to BlendDefault
    T_CHECK(BlendAlpha == BlendDefault);
}

void BlendModeTestsModule::blendModeCustomTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("blend_mode_custom");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(BLEND_SRC_COLOR);

    BlendMode custom;
    custom.colorSrcFactor = BlendMode::Factor::Zero;
    custom.colorDstFactor = BlendMode::Factor::One;
    custom.alphaSrcFactor = BlendMode::Factor::Zero;
    custom.alphaDstFactor = BlendMode::Factor::One;

    RenderStates states;
    states.blend_mode = custom;
    window.clear(BLEND_BG_COLOR);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color(100, 150, 128, 255)));
}
