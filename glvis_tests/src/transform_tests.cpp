#include "glvis_tests/transform_tests.h"
#include "glvis/rectangle.h"

TransformTestsModule::TransformTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto move_test = addTest("move", [&](test::Test& test) { moveTest(test); });
    auto set_origin_test = addTest("set_origin", [&](test::Test& test) { setOriginTest(test); });
    auto rotate_rop_left_test = addTest("rotate_top_left", { set_origin_test }, [&](test::Test& test) { rotateTopLeftTest(test); });
    auto rotate_center_test = addTest("rotate_center", { set_origin_test }, [&](test::Test& test) { rotateCenterTest(test); });
    auto scale_top_left_test = addTest("scale_top_left", { set_origin_test }, [&](test::Test& test) { scaleTopLeftTest(test); });
    auto scale_center_test = addTest("scale_center", { set_origin_test }, [&](test::Test& test) { scaleCenterTest(test); });
}

void TransformTestsModule::moveTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("move");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // move rect 10 pixels right and down
    const Vector2f move_offset = Vector2f(10.0f, 10.0f);
    const Vector2f new_pos = rect.getPosition() + move_offset;
    rect.move(move_offset);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has moved 10 pixels right and down
    image = window.readPixels();
    Vector2i new_pos_int = static_cast<Vector2i>(new_pos);
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int + rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(new_pos_int + rect_size_int), Color::Black, &Color::toString);
}

void TransformTestsModule::setOriginTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("set origin");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // render rect with default origin (0,0)
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2f rect_half_size = rect_size / 2.0f;
    const Vector2i rect_half_size_int = static_cast<Vector2i>(rect_half_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(0, 0), rect_size_int, Color::Red));
    T_COMPARE(image.getPixel(rect_size_int + Vector2i(1, 1)), Color::Black, &Color::toString);

    // set origin to (-10,-10)
    rect.setOrigin(-rect_size);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has visually moved right and down
    image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        rect_size_int,
        rect_size_int * 2,
        Color::Red
    ));
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int * 2 + Vector2i(1, 1)), Color::Black, &Color::toString);
}

void TransformTestsModule::rotateTopLeftTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("rotate without origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // rotate rect 45 degrees around top-left
    rect.setRotation(degrees(45.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check pixels for rotation around top-left
    image = window.readPixels();
    const Vector2f original_rect_center = rect_half_size;
    const Vector2f new_rect_center = Vector2f(sqrt(2.0f) * original_rect_center.x, 0);
    const Vector2i new_rect_center_screen = static_cast<Vector2i>(window_center + new_rect_center);
    const int pixel_offset = 5;
    const Vector2i top_left     = new_rect_center_screen + Vector2i(-pixel_offset    , -pixel_offset    );
    const Vector2i top_right    = new_rect_center_screen + Vector2i( pixel_offset - 1, -pixel_offset    );
    const Vector2i bottom_right = new_rect_center_screen + Vector2i( pixel_offset - 1,  pixel_offset - 1);
    const Vector2i bottom_left  = new_rect_center_screen + Vector2i(-pixel_offset    ,  pixel_offset - 1);
    T_COMPARE(image.getPixel(top_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(top_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(window_center_int), Color::Red, &Color::toString);
}

void TransformTestsModule::rotateCenterTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("rotate with origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // rotate rect 45 degrees around center
    rect.setOrigin(rect_half_size);
    rect.setRotation(degrees(45.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check pixels around screen center
    image = window.readPixels();
    const int pixel_offset = 5;
    const Vector2i top_left     = window_center_int + Vector2i(-pixel_offset    , -pixel_offset    );
    const Vector2i top_right    = window_center_int + Vector2i( pixel_offset - 1, -pixel_offset    );
    const Vector2i bottom_right = window_center_int + Vector2i( pixel_offset - 1,  pixel_offset - 1);
    const Vector2i bottom_left  = window_center_int + Vector2i(-pixel_offset    ,  pixel_offset - 1);
    T_COMPARE(image.getPixel(top_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(top_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_right), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_left), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(window_center_int), Color::Red, &Color::toString);
}

void TransformTestsModule::scaleTopLeftTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("scale without origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // scale rect around top-left
    rect.setScale(2.0f, 2.0f);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has scaled
    image = window.readPixels();
    const Vector2i scaled_rect_start = window_center_int;
    const Vector2i scaled_rect_end = window_center_int + rect_size_int * 2;
    T_WRAP_CONTAINER(checkPixelColor(test, image, scaled_rect_start, scaled_rect_end, Color::Red));

    // check outside
    T_COMPARE(image.getPixel(scaled_rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(scaled_rect_end), Color::Black, &Color::toString);
}

void TransformTestsModule::scaleCenterTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("scale with origin change");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2f rect_half_size = rect_size / 2.0f;
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // center rect on the screen
    view.setPosition(Vector2f());
    window.setView(view);

    // scale rect around center
    rect.setOrigin(rect_half_size);
    rect.setScale(Vector2f(2.0f, 2.0f));
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check that the rectangle has scaled
    image = window.readPixels();
    const Vector2i half_scaled_size = rect_size_int;
    const Vector2i scaled_rect_start = window_center_int - half_scaled_size;
    const Vector2i scaled_rect_end = window_center_int + half_scaled_size;
    T_WRAP_CONTAINER(checkPixelColor(test, image, scaled_rect_start, scaled_rect_end, Color::Red));

    // check outside
    T_COMPARE(image.getPixel(scaled_rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(scaled_rect_end), Color::Black, &Color::toString);
}
