#include "glvx_tests/coordinate_transform_tests.h"

#include "glvx/view.h"

CoordinateTransformTestsModule::CoordinateTransformTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto world_to_screen_identity_test = addTest("world_to_screen_identity", [&](test::Test& test) { worldToScreenIdentityTest(test); });
    auto world_to_screen_pan_test = addTest("world_to_screen_pan", { world_to_screen_identity_test }, [&](test::Test& test) { worldToScreenPanTest(test); });
    auto world_to_screen_zoom_test = addTest("world_to_screen_zoom", { world_to_screen_identity_test }, [&](test::Test& test) { worldToScreenZoomTest(test); });
    auto world_to_screen_rotate_test = addTest("world_to_screen_rotate", { world_to_screen_identity_test }, [&](test::Test& test) { worldToScreenRotateTest(test); });
    auto screen_to_world_identity_test = addTest("screen_to_world_identity", { world_to_screen_identity_test }, [&](test::Test& test) { screenToWorldIdentityTest(test); });
    auto screen_to_world_pan_test = addTest("screen_to_world_pan", { screen_to_world_identity_test }, [&](test::Test& test) { screenToWorldPanTest(test); });
    auto screen_to_world_zoom_test = addTest("screen_to_world_zoom", { screen_to_world_identity_test }, [&](test::Test& test) { screenToWorldZoomTest(test); });
    auto screen_to_world_rotate_test = addTest("screen_to_world_rotate", { screen_to_world_identity_test }, [&](test::Test& test) { screenToWorldRotateTest(test); });
    auto coordinate_round_trip_test = addTest("coordinate_round_trip", { world_to_screen_identity_test, screen_to_world_identity_test }, [&](test::Test& test) { coordinateRoundTripTest(test); });
}

void CoordinateTransformTestsModule::worldToScreenIdentityTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("world to screen identity");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const Vector2f window_center = window.getCenter();
    Vector2i result = window.worldToScreen(window_center);
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    result = window.worldToScreen(Vector2f(0.5f, 0.5f));
    T_VEC2_COMPARE(result, Vector2i(0, 0));

    result = window.worldToScreen(Vector2f(100.5f, 100.5f));
    T_VEC2_COMPARE(result, Vector2i(100, 100));

    result = window.worldToScreen(50.5f, 50.5f);
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    result = window.worldToScreen(Vector2f(25.5f, 25.5f));
    T_VEC2_COMPARE(result, Vector2i(25, 25));
}

void CoordinateTransformTestsModule::worldToScreenPanTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("world to screen pan");
    View view;
    view.setPosition(Vector2f(50.5f, 50.5f));
    window.setView(view);

    // Test with no pan
    Vector2i result = window.worldToScreen(Vector2f(50.5f, 50.5f));
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    // Move view to (60.5, 60.5)
    view.setPosition(Vector2f(60.5f, 60.5f));
    window.setView(view);

    result = window.worldToScreen(Vector2f(60.5f, 60.5f));
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    result = window.worldToScreen(Vector2f(50.5f, 50.5f));
    T_VEC2_COMPARE(result, Vector2i(40, 40));

    result = window.worldToScreen(Vector2f(100.5f, 100.5f));
    T_VEC2_COMPARE(result, Vector2i(90, 90));
}

void CoordinateTransformTestsModule::worldToScreenZoomTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("world to screen zoom");
    View view;
    view.setPosition(Vector2f(50.5f, 50.5f));
    view.setZoom(1.0f);
    window.setView(view);

    Vector2i result = window.worldToScreen(Vector2f(50.5f, 50.5f));
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    view.setZoom(2.0f);
    window.setView(view);

    result = window.worldToScreen(Vector2f(25.5f, 25.5f));
    T_VEC2_COMPARE(result, Vector2i(0, 0));

    result = window.worldToScreen(Vector2f(50.5f, 50.5f));
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    result = window.worldToScreen(Vector2f(75.5f, 75.5f));
    T_VEC2_COMPARE(result, Vector2i(100, 100));

    view.setZoom(0.5f);
    window.setView(view);

    // With zoom=0.5, world range [-50, 150] maps to screen [0, 100]
    // World (-49.5, -49.5) maps to screen (0, 0)
    result = window.worldToScreen(Vector2f(-49.5f, -49.5f));
    T_VEC2_COMPARE(result, Vector2i(0, 0));

    // World (50.5, 50.5) still maps to screen (50, 50)
    result = window.worldToScreen(Vector2f(50.5f, 50.5f));
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    // World (149.5, 149.5) maps to screen (99, 99)
    result = window.worldToScreen(Vector2f(149.5f, 149.5f));
    T_VEC2_COMPARE(result, Vector2i(99, 99));

    // World (150.5, 150.5) maps to screen (100, 100)
    result = window.worldToScreen(Vector2f(150.5f, 150.5f));
    T_VEC2_COMPARE(result, Vector2i(100, 100));
}

void CoordinateTransformTestsModule::worldToScreenRotateTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("world to screen rotate");
    View view;

    // Set view at (50.5, 50.5) with rotation = 0
    view.setPosition(Vector2f(50.0f, 50.0f));
    view.setRotation(Angle::fromDegrees(0.0f));
    window.setView(view);

    // Test with no rotation

    // top left
    Vector2i result = window.worldToScreen(Vector2f(0.5f, 0.5f));
    T_VEC2_COMPARE(result, Vector2i(0, 0));

    // top right
    result = window.worldToScreen(Vector2f(99.5f, 0.5f));
    T_VEC2_COMPARE(result, Vector2i(99, 0));

    // bottom right
    result = window.worldToScreen(Vector2f(99.5f, 99.5f));
    T_VEC2_COMPARE(result, Vector2i(99, 99));

    // bottom left
    result = window.worldToScreen(Vector2f(0.5f, 99.5f));
    T_VEC2_COMPARE(result, Vector2i(0, 99));

    // center
    result = window.worldToScreen(Vector2f(50.5f, 50.5f));
    T_VEC2_COMPARE(result, Vector2i(50, 50));

    // Rotate 90 degrees clockwise
    view.setRotation(Angle::fromDegrees(90.0f));
    window.setView(view);

    // With 90-degree rotation, the coordinate system rotates

    // top left
    result = window.worldToScreen(Vector2f(0.5f, 0.5f));
    T_VEC2_COMPARE(result, Vector2i(99, 0));

    // top right
    result = window.worldToScreen(Vector2f(99.5f, 0.5f));
    T_VEC2_COMPARE(result, Vector2i(99, 99));

    // bottom right
    result = window.worldToScreen(Vector2f(99.5f, 99.5f));
    T_VEC2_COMPARE(result, Vector2i(0, 99));

    // bottom left
    result = window.worldToScreen(Vector2f(0.5f, 99.5f));
    T_VEC2_COMPARE(result, Vector2i(0, 0));

    // center
    result = window.worldToScreen(Vector2f(50.5f, 50.5f));
    T_VEC2_COMPARE(result, Vector2i(49, 50));
}

void CoordinateTransformTestsModule::screenToWorldIdentityTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("screen to world identity");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    Vector2f result = window.screenToWorld(50, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.5f, 50.5f));

    result = window.screenToWorld(Vector2i(0, 0));
    T_VEC2_APPROX_COMPARE(result, Vector2f(0.5f, 0.5f));

    result = window.screenToWorld(Vector2i(100, 100));
    T_VEC2_APPROX_COMPARE(result, Vector2f(100.5f, 100.5f));

    result = window.screenToWorld(25, 25);
    T_VEC2_APPROX_COMPARE(result, Vector2f(25.5f, 25.5f));
}

void CoordinateTransformTestsModule::screenToWorldPanTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("screen to world pan");
    View view;

    view.setPosition(window.getCenter());
    window.setView(view);

    Vector2f result = window.screenToWorld(50, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.5f, 50.5f));

    // Test with different pan position
    view.setPosition(Vector2f(60.0f, 60.0f));
    window.setView(view);

    result = window.screenToWorld(50, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(60.5f, 60.5f));

    result = window.screenToWorld(40, 40);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.5f, 50.5f));

    result = window.screenToWorld(60, 60);
    T_VEC2_APPROX_COMPARE(result, Vector2f(70.5f, 70.5f));
}

void CoordinateTransformTestsModule::screenToWorldZoomTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("screen to world zoom");
    View view;
    view.setPosition(Vector2f(50.0f, 50.0f));
    view.setZoom(1.0f);
    window.setView(view);

    Vector2f result = window.screenToWorld(50, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.5f, 50.5f));

    // Test with zoom=2
    view.setZoom(2.0f);
    window.setView(view);

    // with zoom=2 pixel center offset turns into 0.25
    result = window.screenToWorld(50, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.25f, 50.25f));

    result = window.screenToWorld(0, 0);
    T_VEC2_APPROX_COMPARE(result, Vector2f(25.25f, 25.25f));

    result = window.screenToWorld(100, 100);
    T_VEC2_APPROX_COMPARE(result, Vector2f(75.25f, 75.25f));

    // Test with zoom=0.5
    view.setZoom(0.5f);
    window.setView(view);

    // with zoom=0.5 pixel center offset turns into 1.0
    result = window.screenToWorld(0, 0);
    T_VEC2_APPROX_COMPARE(result, Vector2f(-49.0f, -49.0f));

    result = window.screenToWorld(100, 100);
    T_VEC2_APPROX_COMPARE(result, Vector2f(151.0f, 151.0f));
}

void CoordinateTransformTestsModule::screenToWorldRotateTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("screen to world rotate");
    View view;
    view.setPosition(Vector2f(50.0f, 50.0f));
    view.setRotation(Angle::fromDegrees(0.0f));
    window.setView(view);

    Vector2f result = window.screenToWorld(60, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(60.5f, 50.5f));

    result = window.screenToWorld(50, 60);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.5f, 60.5f));

    // Rotate 90 degrees clockwise
    view.setRotation(Angle::fromDegrees(90.0f));
    window.setView(view);

    result = window.screenToWorld(50, 100);
    T_VEC2_APPROX_COMPARE(result, Vector2f(100.5f, 49.5f));

    result = window.screenToWorld(100, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.5f, -0.5f));

    result = window.screenToWorld(50, 0);
    T_VEC2_APPROX_COMPARE(result, Vector2f(0.5f, 49.5f));

    result = window.screenToWorld(0, 50);
    T_VEC2_APPROX_COMPARE(result, Vector2f(50.5f, 99.5f));
}

void CoordinateTransformTestsModule::coordinateRoundTripTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("coordinate round trip");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    // Test round-trip: screenToWorld(worldToScreen(x)) should return x
    const Vector2f originalWorld(25.0f, 75.0f);
    Vector2i screen = window.worldToScreen(originalWorld);
    Vector2f roundTripped = window.screenToWorld(screen);
    T_VEC2_APPROX_COMPARE(roundTripped, Vector2f(25.5f, 75.5f));

    // Test round-trip: worldToScreen(screenToWorld(x)) should return x
    const Vector2i originalScreen(30, 70);
    Vector2f toWorld = window.screenToWorld(originalScreen);
    Vector2i backToScreen = window.worldToScreen(toWorld);
    T_VEC2_COMPARE(backToScreen, originalScreen);

    // Test with different view position
    view.setPosition(Vector2f(25.0f, 25.0f));
    window.setView(view);

    const Vector2f world2(50.0f, 50.0f);
    screen = window.worldToScreen(world2);
    roundTripped = window.screenToWorld(screen);
    T_VEC2_APPROX_COMPARE(roundTripped, Vector2f(50.5f, 50.5f));
}
