#include "glvx_tests/blend_mode_tests.h"

#include "glvx/blend_mode.h"
#include "glvx/rectangle.h"
#include "glvx/render_states.h"

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
    window.clear(Color::Black);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);

    RenderStates states;
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Red));
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
    rect.setColor(Color(255, 0, 0, 255));

    RenderStates states;
    states.blend_mode = BlendAlpha;
    window.clear(Color::Black);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Red));
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
    rect.setColor(Color(128, 0, 0, 255));

    RenderStates states;
    states.blend_mode = BlendAdd;
    window.clear(Color::Black);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color(128, 0, 0, 255)));
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
    rect.setColor(Color::Red);

    RenderStates states;
    states.blend_mode = BlendMultiply;
    window.clear(Color::Black);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Black));
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
    rect.setColor(Color::Red);

    RenderStates states;
    states.blend_mode = BlendNone;
    window.clear(Color::White);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Red));
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
    T_CHECK(!(BlendAlpha == BlendDefault));
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
    rect.setColor(Color::Red);

    BlendMode custom;
    custom.colorSrcFactor = BlendMode::Factor::Zero;
    custom.colorDstFactor = BlendMode::Factor::One;
    custom.alphaSrcFactor = BlendMode::Factor::Zero;
    custom.alphaDstFactor = BlendMode::Factor::One;

    RenderStates states;
    states.blend_mode = custom;
    window.clear(Color::Green);
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Green));
}
