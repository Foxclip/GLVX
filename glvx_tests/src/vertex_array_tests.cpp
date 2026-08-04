#include "glvx_tests/vertex_array_tests.h"

#include "glvx/vertex_array.h"

VertexArrayTestsModule::VertexArrayTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto vertex_array_triangle_test = addTest("vertex_array_triangle", [&](test::Test& test) { vertexArrayTriangleTest(test); });
    auto vertex_array_line_test = addTest("vertex_array_line", { vertex_array_triangle_test }, [&](test::Test& test) { vertexArrayLineTest(test); });
    auto vertex_array_modify_test = addTest("vertex_array_modify", { vertex_array_triangle_test }, [&](test::Test& test) { vertexArrayModifyTest(test); });
}

void VertexArrayTestsModule::vertexArrayTriangleTest(test::Test& test) {
   window.setSize(WINDOW_SIZE);
   window.setTitle("vertex array triangle");
   View view;
   Vector2f window_center = window.getCenter();
   view.setPosition(window_center);
   window.setView(view);
   window.clear(Color::Black);

   // Render a triangle
   const int triangle_vertex_count = 3;
   VertexArray triangle(PrimitiveType::Triangles, triangle_vertex_count);
   const Vector2f triangle_base_left = Vector2f(0, 0);
   const Vector2f triangle_base_right = Vector2f(10, 0);
   const Vector2f triangle_top = Vector2f(5, 10);
   triangle[0] = Vertex(triangle_base_left, Color::Red, Vector2f(0, 0));
   triangle[1] = Vertex(triangle_base_right, Color::Red, Vector2f(0, 0));
   triangle[2] = Vertex(triangle_top, Color::Red, Vector2f(0, 0));
   window.draw(triangle);
   window.display();

   // Check that the triangle is rendered correctly
   Image image = window.readPixels();
   const Vector2i triangle_center_check = Vector2i(5, 5);
   const Vector2i triangle_left_check = static_cast<Vector2i>(triangle_base_left + Vector2f(2, 2));
   const Vector2i triangle_right_check = static_cast<Vector2i>(triangle_base_right + Vector2f(-2, 2));
   const Vector2i outside_left = static_cast<Vector2i>(triangle_base_left + Vector2f(0, 10));
   const Vector2i outside_right = static_cast<Vector2i>(triangle_base_right);
   const Vector2i outside_far = Vector2i(15, 15);
   T_COMPARE(image.getPixel(triangle_center_check), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(triangle_left_check), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(triangle_right_check), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(outside_left), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(outside_right), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(outside_far), Color::Black, &Color::toString);
}

void VertexArrayTestsModule::vertexArrayLineTest(test::Test& test) {
   window.setSize(WINDOW_SIZE);
   window.setTitle("vertex array line");
   View view;
   Vector2f window_center = window.getCenter();
   view.setPosition(window_center);
   window.setView(view);
   window.clear(Color::Black);

   // Render a line
   const int line_vertex_count = 2;
   VertexArray line(PrimitiveType::Lines, line_vertex_count);
   const Vector2f line_start = Vector2f(10, 50);
   const Vector2f line_end = Vector2f(90, 50);
   line[0] = Vertex(line_start, Color::Red, Vector2f(0, 0));
   line[1] = Vertex(line_end, Color::Red, Vector2f(0, 0));
   window.draw(line);
   window.display();

   // Check line pixels
   Image image = window.readPixels();
   const Vector2i line_start_int = static_cast<Vector2i>(line_start);
   const Vector2i line_mid = static_cast<Vector2i>((line_start + line_end) / 2.0f);
   const Vector2i line_end_int = static_cast<Vector2i>(line_end);
   T_COMPARE(image.getPixel(line_start_int), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(line_mid), Color::Red, &Color::toString);
   T_COMPARE(image.getPixel(line_end_int - Vector2i(1, 0)), Color::Red, &Color::toString);

   // Check line edges on the outside
   T_COMPARE(image.getPixel(line_start_int - Vector2i(1, 0)), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(line_end_int), Color::Black, &Color::toString);

   // Check above and below
   const Vector2i above_line = line_mid - Vector2i(0, 1);
   const Vector2i below_line = line_mid + Vector2i(0, 1);
   T_COMPARE(image.getPixel(above_line), Color::Black, &Color::toString);
   T_COMPARE(image.getPixel(below_line), Color::Black, &Color::toString);
}

void VertexArrayTestsModule::vertexArrayModifyTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("vertex array triangle");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Render initial triangle
    const int initial_vertex_count = 3;
    VertexArray vertex_array(PrimitiveType::Triangles, initial_vertex_count);
    const Vector2f triangle_base_left = Vector2f(0, 0);
    const Vector2f triangle_base_right = Vector2f(10, 0);
    const Vector2f triangle_top = Vector2f(5, 10);
    vertex_array[0] = Vertex(triangle_base_left, Color::Red, Vector2f(0, 0));
    vertex_array[1] = Vertex(triangle_base_right, Color::Red, Vector2f(0, 0));
    vertex_array[2] = Vertex(triangle_top, Color::Red, Vector2f(0, 0));
    window.draw(vertex_array);
    window.display();

    // Check initial pixels
    Image initial_image = window.readPixels();
    const Vector2i triangle_center_check = Vector2i(5, 5);
    const Vector2i triangle_left_check = Vector2i(2, 2);
    const Vector2i outside_initial = Vector2i(15, 5);
    T_COMPARE(initial_image.getPixel(triangle_center_check), Color::Red, &Color::toString);
    T_COMPARE(initial_image.getPixel(triangle_left_check), Color::Red, &Color::toString);
    T_COMPARE(initial_image.getPixel(outside_initial), Color::Black, &Color::toString);

    // Add additional vertices to the VertexArray
    const Vector2f second_triangle_base_left = Vector2f(20, 0);
    const Vector2f second_triangle_base_right = Vector2f(30, 0);
    const Vector2f second_triangle_top = Vector2f(25, 10);
    vertex_array.append(Vertex(second_triangle_base_left, Color::Red, Vector2f(0, 0)));
    vertex_array.append(Vertex(second_triangle_base_right, Color::Red, Vector2f(0, 0)));
    vertex_array.append(Vertex(second_triangle_top, Color::Red, Vector2f(0, 0)));
    window.clear(Color::Black);
    window.draw(vertex_array);
    window.display();

    // Check updated pixels
    Image updated_image = window.readPixels();
    const Vector2i second_triangle_center_check = Vector2i(25, 5);
    const Vector2i second_triangle_left_check = Vector2i(22, 2);
    T_COMPARE(initial_image.getPixel(second_triangle_center_check), Color::Black, &Color::toString);
    T_COMPARE(initial_image.getPixel(second_triangle_left_check), Color::Black, &Color::toString);
    T_COMPARE(updated_image.getPixel(triangle_center_check), Color::Red, &Color::toString); // Still red
    T_COMPARE(updated_image.getPixel(second_triangle_center_check), Color::Red, &Color::toString); // New area
    T_COMPARE(updated_image.getPixel(second_triangle_left_check), Color::Red, &Color::toString); // New area
    T_COMPARE(updated_image.getPixel(outside_initial), Color::Black, &Color::toString); // Still black

    // Delete the added vertices (removing the second triangle)
    vertex_array.resize(3);
    window.clear(Color::Black);
    window.draw(vertex_array);
    window.display();

    // Check final image matches initial
    Image final_image = window.readPixels();
    T_WRAP_CONTAINER(compareImages(test, final_image, initial_image));
}
