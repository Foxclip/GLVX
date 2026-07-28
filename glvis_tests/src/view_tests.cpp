#include "glvis_tests/view_tests.h"
#include "glvis/rectangle.h"

ViewTestsModule::ViewTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto view_pan_test = addTest("view_pan", [&](test::Test& test) { viewPanTest(test); });
    auto view_zoom_test = addTest("view_zoom", [&](test::Test& test) { viewZoomTest(test); });
    auto view_rotate_test = addTest("view_rotate", [&](test::Test& test) { viewRotateTest(test); });
}

void ViewTestsModule::viewPanTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("view pan");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // move View 10 pixels up and left
    const Vector2f pan_offset = Vector2f(-10.0f, -10.0f);
    view.setPosition(view.getPosition() + pan_offset);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check that the View has panned 10 pixels up and left
    const Vector2i panned_rect_start = Vector2i(10, 10);
    const Vector2i panned_rect_end = panned_rect_start + rect_size_int;
    T_COMPARE(image.getPixel(0, 0), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_start), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_end - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(panned_rect_end), Color::Black, &Color::toString);
}

void ViewTestsModule::viewZoomTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("view zoom");
    View view;
    const Vector2f initial_view_pos = Vector2f(5.0f, 5.0f);
    view.setPosition(initial_view_pos);
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
    const Vector2i initial_check_start = Vector2i(45, 45);
    const Vector2i initial_check_end = Vector2i(55, 55);
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        initial_check_start.x,
        initial_check_start.y,
        initial_check_end.x,
        initial_check_end.y,
        Color::Red
    ));

    // zoom in by factor 2
    const float zoom_factor = 2.0f;
    view.setZoom(zoom_factor);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();
    image = window.readPixels();

    // check that the View has zoomed in
    const Vector2i zoomed_check_start = Vector2i(40, 40);
    const Vector2i zoomed_check_end = Vector2i(60, 60);
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        zoomed_check_start.x,
        zoomed_check_start.y,
        zoomed_check_end.x,
        zoomed_check_end.y,
        Color::Red
    ));

    // check outside
    T_COMPARE(image.getPixel(zoomed_check_start - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(zoomed_check_end), Color::Black, &Color::toString);
}

void ViewTestsModule::viewRotateTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("view rotation");
    View view;
    Vector2f window_center = window.getCenter();
    Vector2i window_center_int = static_cast<Vector2i>(window_center);
    view.setPosition(0.0f, 0.0f);
    window.setView(view);
    window.clear(Color::Black);

    // render rect
    const Vector2f rect_size = Vector2f(20.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    rect.setPosition(0.0f, 0.0f);
    window.draw(rect);
    window.display();

    // check initial rectangle position
    Image image = window.readPixels();
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    T_WRAP_CONTAINER(checkPixelColor(
        test, image, window_center_int, window_center_int + rect_size_int, Color::Red)
    );

    // rotate View 90 degrees
    const Vector2f rotated_view_pos = Vector2f(0.0f, 0.0f);
    const Vector2f rotated_rect_size = Vector2f(rect_size.y, rect_size.x);
    const Vector2i rotated_rect_size_int = static_cast<Vector2i>(rotated_rect_size);
    view.setPosition(rotated_view_pos);
    const Angle rotation_angle = Angle::fromDegrees(90.0f);
    view.setRotation(rotation_angle);
    window.setView(view);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // check pixels around screen center
    image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        window_center_int.x - rotated_rect_size_int.x,
        window_center_int.y,
        window_center_int.x,
        window_center_int.y + rotated_rect_size_int.y,
        Color::Red
    ));
    // check outside
    T_COMPARE(image.getPixel(window_center_int - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(window_center_int + rotated_rect_size_int), Color::Black, &Color::toString);
}
