#include "glvis_tests/shape_tests.h"
#include "glvis/circle.h"

using namespace glvis;

ShapeTestsModule::ShapeTestsModule(
    const std::string& name,
    test::TestModule *parent,
    const std::vector<test::TestNode *>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto rectangle_test = addTest("rectangle", [&](test::Test& test) { rectangleTest(test); });
    auto rectangle_set_size_test = addTest("retcangle_set_size", { rectangle_test }, [&](test::Test& test) { rectangleSetSizeTest(test); });
    auto circle_test = addTest("circle", [&](test::Test& test) { circleTest(test); });
    auto circle_set_radius_test = addTest("circle_set_radius", { circle_test }, [&](test::Test& test) { circleSetRadiusTest(test); });
}

void ShapeTestsModule::rectangleTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("rectangle");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // Render a rectangle
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // Check that the rectangle is rendered correctly
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(rect_size) - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(static_cast<Vector2i>(rect_size)), Color::Black, &Color::toString);
}

void ShapeTestsModule::rectangleSetSizeTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("rectangle set size");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // Create a rectangle with initial size
    const Vector2f initial_size = Vector2f(10.0f, 10.0f);
    Rectangle rect(initial_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // Check that the rectangle is rendered correctly with initial size
    Image image = window.readPixels();
    Vector2i rect_size_int = static_cast<Vector2i>(initial_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // Change the size using setSize()
    const Vector2f new_size = Vector2f(20.0f, 15.0f);
    rect.setSize(new_size);
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // Check that the rectangle is rendered correctly with new size
    image = window.readPixels();
    Vector2i new_size_int = static_cast<Vector2i>(new_size);
    T_COMPARE(image.getPixel(0, 0), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(new_size_int - Vector2i(1, 1)), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(new_size_int), Color::Black, &Color::toString);
}

void ShapeTestsModule::circleTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("circle");
    View view;
    const Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Render a circle with 4 vertices
    const float circle_radius = 5.5f;
    const Vector2f circle_center(circle_radius, circle_radius);
    Circle circle(circle_radius, 4); // 4 segments = diamond shape
    circle.setColor(Color::Red);
    window.draw(circle);
    window.display();

    // Check the 4 diamond vertices (all should be Red - on the diamond edges)
    Image image = window.readPixels();
    const float radius_offset = std::floor(circle_radius);
    const Vector2i top_check = static_cast<Vector2i>(
        circle_center + Vector2f(0, -radius_offset)
    );
    const Vector2i right_check = static_cast<Vector2i>(
        circle_center + Vector2f(radius_offset, 0)
    );
    const Vector2i bottom_check = static_cast<Vector2i>(
        circle_center + Vector2f(0, radius_offset)
    );
    const Vector2i left_check = static_cast<Vector2i>(
        circle_center + Vector2f(-radius_offset, 0)
    );
    T_COMPARE(image.getPixel(top_check), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(right_check), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(bottom_check), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(left_check), Color::Red, &Color::toString);

    // Check diagonal pixels from top-left to bottom-right
    const float offset = static_cast<float>(circle_radius / std::sqrt(2));
    const Vector2i top_left_check = static_cast<Vector2i>(
        circle_center + Vector2f(-offset, -offset)
    );
    const Vector2i bottom_right_check = static_cast<Vector2i>(
        circle_center + Vector2f(offset, offset)
    );
    const Vector2i top_right_check = static_cast<Vector2i>(
        circle_center + Vector2f(offset, -offset)
    );
    const Vector2i bottom_left_check = static_cast<Vector2i>(
        circle_center + Vector2f(-offset, offset)
    );
    T_COMPARE(image.getPixel(top_left_check), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_right_check), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(top_right_check), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(bottom_left_check), Color::Black, &Color::toString);

    // Check a pixel outside the diamond (should be Black)
    const Vector2i outside_check = static_cast<Vector2i>(
        circle_center + Vector2f(circle_radius, circle_radius) + Vector2f(1, 1)
    );
    T_COMPARE(image.getPixel(outside_check), Color::Black, &Color::toString);
}

void ShapeTestsModule::circleSetRadiusTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("circle_set_radius");
    View view;
    const Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Render a circle with initial radius
    const float initial_radius = 5.5f;
    const Vector2f circle_center(initial_radius, initial_radius);
    Circle circle(initial_radius, 4); // 4 segments = diamond shape
    circle.setColor(Color::Red);
    window.draw(circle);
    window.display();

    // Check the initial circle (smaller diamond)
    Image image = window.readPixels();
    const float initial_radius_offset = std::floor(initial_radius);
    const Vector2i initial_right_check = static_cast<Vector2i>(
        circle_center + Vector2f(initial_radius_offset, 0)
    );
    const Vector2i initial_bottom_check = static_cast<Vector2i>(
        circle_center + Vector2f(0, initial_radius_offset)
    );
    T_COMPARE(image.getPixel(initial_right_check), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(initial_bottom_check), Color::Red, &Color::toString);

    // Check a pixel that should be outside the initial circle but inside the new one
    const float new_radius = 10.5f;
    const float mid_radius = (initial_radius + new_radius) / 2.0f;
    const Vector2i mid_check = static_cast<Vector2i>(
        circle_center + Vector2f(mid_radius, 0)
    );
    T_COMPARE(image.getPixel(mid_check), Color::Black, &Color::toString);

    // Change the radius
    circle.setRadius(new_radius);
    window.clear(Color::Black);
    window.draw(circle);
    window.display();

    // Check the circle with new radius (larger diamond)
    image = window.readPixels();
    const float new_radius_offset = std::floor(new_radius);
    const Vector2i new_right_check = static_cast<Vector2i>(
        circle_center + Vector2f(new_radius_offset, 0)
    );
    const Vector2i new_bottom_check = static_cast<Vector2i>(
        circle_center + Vector2f(0, new_radius_offset)
    );
    T_COMPARE(image.getPixel(new_right_check), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(new_bottom_check), Color::Red, &Color::toString);

    // Check that the midpoint is now red
    T_COMPARE(image.getPixel(mid_check), Color::Red, &Color::toString);

    // Check a pixel outside the new circle
    const Vector2i outside_check = static_cast<Vector2i>(
        circle_center + Vector2f(new_radius_offset + 1, 0)
    );
    T_COMPARE(image.getPixel(outside_check), Color::Black, &Color::toString);
}
