#include "glvx_tests/antialiasing_tests.h"
#include "glvx/rectangle.h"
#include "glvx/view.h"
#include "glvx/render_texture.h"

Window aaWindow;

AntialiasingTestsModule::AntialiasingTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    test::Test* window_aa_solid_rect_test = addTest("window_aa_solid_rect", [&](test::Test& test) { windowAASolidRectTest(test); });
    test::Test* window_aa_subpixel_rect_test = addTest("window_aa_subpixel_rect", { window_aa_solid_rect_test }, [&](test::Test& test) { windowAASubpixelRectTest(test); });
    test::Test* rt_aa_solid_rect_test = addTest("rt_aa_solid_rect", [&](test::Test& test) { renderTextureAASolidRectTest(test); });
    test::Test* rt_aa_subpixel_rect_test = addTest("rt_aa_subpixel_rect", { rt_aa_solid_rect_test }, [&](test::Test& test) { renderTextureAASubpixelRectTest(test); });
}

void AntialiasingTestsModule::beforeRunModule() {
    aaWindow.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "aa window", AA_WINDOW_SAMPLES);
}

void AntialiasingTestsModule::afterRunModule() {
    aaWindow.close();
}

void AntialiasingTestsModule::windowAASolidRectTest(test::Test& test) {
    aaWindow.setSize(WINDOW_SIZE);
    aaWindow.setTitle("window aa solid rect");
    View view;
    view.setPosition(aaWindow.getCenter());
    aaWindow.setView(view);
    aaWindow.clear(Color::Black);

    const Vector2f rect_size(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    aaWindow.draw(rect);
    aaWindow.display();

    Image image = aaWindow.readPixels();
    const Vector2i rect_size_int(static_cast<Vector2i>(rect_size));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, Vector2i(0, 0), rect_size_int, Color::Red
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y), Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y), Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, rect_size_int, WINDOW_SIZE, Color::Black
    ));
}

void AntialiasingTestsModule::windowAASubpixelRectTest(test::Test& test) {
    aaWindow.setSize(WINDOW_SIZE);
    aaWindow.setTitle("window aa subpixel rect");
    View view;
    view.setPosition(aaWindow.getCenter());
    aaWindow.setView(view);
    aaWindow.clear(Color::Black);

    const Vector2f rect_size(10.0f, 10.0f);
    const Vector2i rect_size_int(static_cast<Vector2i>(rect_size));
    Rectangle rect(rect_size);
    rect.setPosition(0.5f, 0.5f); // move rect so that its border is in the middle of a pixel
    rect.setColor(Color::Red);
    aaWindow.draw(rect);
    aaWindow.display();

    Image image = aaWindow.readPixels();
    // top left border
    int red_top_left = image.getPixel(0, 0).r;
    T_CHECK(
        red_top_left > 0 && red_top_left < 255,
        "Top left red is not strictly between 0 and 255: " + std::to_string(red_top_left)
    );
    // top left inside
    T_COMPARE(image.getPixel(1, 1), Color::Red, &Color::toString);
    // bottom right inside
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    // bottom right border
    int red_bottom_right = image.getPixel(rect_size_int).r;
    T_CHECK(
        red_bottom_right > 0 && red_bottom_right < 255,
        "Bottom right red is not strictly between 0 and 255: " + std::to_string(red_bottom_right)
    );
    // bottom right outside
    T_COMPARE(image.getPixel(rect_size_int + Vector2i(1, 1)), Color::Black, &Color::toString);
}

void AntialiasingTestsModule::renderTextureAASolidRectTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y, AA_WINDOW_SAMPLES);
    View view;
    view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    render_texture.setView(view);
    render_texture.clear(Color::Black);

    const Vector2f rect_size(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    render_texture.draw(rect);
    render_texture.display();

    Image image = render_texture.readPixels();
    const Vector2i rect_size_int(static_cast<Vector2i>(rect_size));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, Vector2i(0, 0), rect_size_int, Color::Red
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, Vector2i(rect_size_int.x, 0), Vector2i(WINDOW_SIZE.x, rect_size_int.y), Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, Vector2i(0, rect_size_int.y), Vector2i(rect_size_int.x, WINDOW_SIZE.y), Color::Black
    ));
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, rect_size_int, WINDOW_SIZE, Color::Black
    ));
}

void AntialiasingTestsModule::renderTextureAASubpixelRectTest(test::Test& test) {
    RenderTexture render_texture(WINDOW_SIZE.x, WINDOW_SIZE.y, AA_WINDOW_SAMPLES);
    View view;
    view.setPosition(static_cast<Vector2f>(WINDOW_SIZE) / 2.0f);
    render_texture.setView(view);
    render_texture.clear(Color::Black);

    const Vector2f rect_size(10.0f, 10.0f);
    const Vector2i rect_size_int(static_cast<Vector2i>(rect_size));
    Rectangle rect(rect_size);
    rect.setPosition(0.5f, 0.5f);
    rect.setColor(Color::Red);
    render_texture.draw(rect);
    render_texture.display();

    Image image = render_texture.readPixels();
    // top left border
    int red_top_left = image.getPixel(0, 0).r;
    T_CHECK(
        red_top_left > 0 && red_top_left < 255,
        "Top left red is not strictly between 0 and 255: " + std::to_string(red_top_left)
    );
    // top left inside
    T_COMPARE(image.getPixel(1, 1), Color::Red, &Color::toString);
    // bottom right inside
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    // bottom right border
    int red_bottom_right = image.getPixel(rect_size_int).r;
    T_CHECK(
        red_bottom_right > 0 && red_bottom_right < 255,
        "Bottom right red is not strictly between 0 and 255: " + std::to_string(red_bottom_right)
    );
    // bottom right outside
    T_COMPARE(image.getPixel(rect_size_int + Vector2i(1, 1)), Color::Black, &Color::toString);
}
