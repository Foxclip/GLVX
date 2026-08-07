#include "glvx_tests/vertex_buffer_tests.h"

#include "glvx/shader.h"
#include "glvx/vertex.h"
#include "glvx/vertex_buffer.h"

using namespace glvx;

// Helper class to make VertexBuffer drawable for testing
class VertexBufferDrawable : public Drawable {
public:
    explicit VertexBufferDrawable(VertexBuffer& vb) : m_vertex_buffer(vb) {
        m_shader = common::defaultShader;
    }

    const VertexBuffer& getVertexBuffer() const override {
        return m_vertex_buffer;
    }

    Transform getTransform() const override {
        return Transform();
    }

private:
    VertexBuffer& m_vertex_buffer;
};

VertexBufferTestsModule::VertexBufferTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto vertex_buffer_render_test = addTest("vertex_buffer_render", [&](test::Test& test) { vertexBufferRenderTest(test); });
    auto vertex_buffer_update_test = addTest("vertex_buffer_update", { vertex_buffer_render_test }, [&](test::Test& test) { vertexBufferUpdateTest(test); });
    auto vertex_buffer_partial_update_test = addTest("vertex_buffer_partial_update", { vertex_buffer_update_test }, [&](test::Test& test) { vertexBufferPartialUpdateTest(test); });
}

void VertexBufferTestsModule::vertexBufferRenderTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("vertex buffer render");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Create a VertexBuffer with initial vertices (red triangle)
    VertexBuffer vertexBuffer(PrimitiveType::Triangles);
    const std::size_t initialVertexCount = 3;
    T_ASSERT(vertexBuffer.create(initialVertexCount));

    const Vector2f triangle_base_left = Vector2f(0, 0);
    const Vector2f triangle_base_right = Vector2f(10, 0);
    const Vector2f triangle_top = Vector2f(5, 10);

    std::vector<Vertex> initialVertices = {
        Vertex(triangle_base_left, Color::Red, Vector2f(0, 0)),
        Vertex(triangle_base_right, Color::Red, Vector2f(0, 0)),
        Vertex(triangle_top, Color::Red, Vector2f(0, 0))
    };

    T_ASSERT(vertexBuffer.update(initialVertices));

    // Create drawable wrapper and render
    VertexBufferDrawable drawable(vertexBuffer);
    window.draw(drawable);
    window.display();

    // Check initial pixels (red triangle at position 0-10)
    Image image = window.readPixels();
    const Vector2f triangle_center = (triangle_base_left + triangle_base_right + triangle_top) / 3.0f;
    const Vector2i triangle_center_check = static_cast<Vector2i>(triangle_center);
    const Vector2i triangle_left_check = static_cast<Vector2i>(triangle_base_left + Vector2f(2, 2));
    const Vector2i outside_initial = static_cast<Vector2i>(triangle_base_right + Vector2f(5, 0));
    T_COMPARE(image.getPixel(triangle_center_check), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(triangle_left_check), Color::Red, &Color::toString);
    T_COMPARE(image.getPixel(outside_initial), Color::Black, &Color::toString);
}

void VertexBufferTestsModule::vertexBufferUpdateTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("vertex buffer update");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Create a VertexBuffer with initial vertices (red triangle)
    VertexBuffer vertexBuffer(PrimitiveType::Triangles);
    const std::size_t initialVertexCount = 3;
    T_ASSERT(vertexBuffer.create(initialVertexCount));

    const Vector2f triangle_base_left = Vector2f(0, 0);
    const Vector2f triangle_base_right = Vector2f(10, 0);
    const Vector2f triangle_top = Vector2f(5, 10);

    std::vector<Vertex> initialVertices = {
        Vertex(triangle_base_left, Color::Red, Vector2f(0, 0)),
        Vertex(triangle_base_right, Color::Red, Vector2f(0, 0)),
        Vertex(triangle_top, Color::Red, Vector2f(0, 0))
    };

    T_ASSERT(vertexBuffer.update(initialVertices));

    // Create drawable wrapper and render
    VertexBufferDrawable drawable(vertexBuffer);
    window.draw(drawable);
    window.display();

    // Check initial pixels (red triangle at position 0-10)
    Image initial_image = window.readPixels();
    const Vector2f triangle_center = (triangle_base_left + triangle_base_right + triangle_top) / 3.0f;
    const Vector2i triangle_center_check = static_cast<Vector2i>(triangle_center);
    const Vector2i triangle_left_check = static_cast<Vector2i>(triangle_base_left + Vector2f(2, 2));
    const Vector2i outside_initial = static_cast<Vector2i>(triangle_base_right + Vector2f(5, 0));
    T_COMPARE(initial_image.getPixel(triangle_center_check), Color::Red, &Color::toString);
    T_COMPARE(initial_image.getPixel(triangle_left_check), Color::Red, &Color::toString);
    T_COMPARE(initial_image.getPixel(outside_initial), Color::Black, &Color::toString);

    // Update with new vertices (blue triangle at different position)
    const Vector2f new_triangle_base_left = Vector2f(20, 0);
    const Vector2f new_triangle_base_right = Vector2f(30, 0);
    const Vector2f new_triangle_top = Vector2f(25, 10);

    std::vector<Vertex> newVertices = {
        Vertex(new_triangle_base_left, Color::Blue, Vector2f(0, 0)),
        Vertex(new_triangle_base_right, Color::Blue, Vector2f(0, 0)),
        Vertex(new_triangle_top, Color::Blue, Vector2f(0, 0))
    };

    T_ASSERT(vertexBuffer.update(newVertices));

    // Render updated buffer
    window.clear(Color::Black);
    window.draw(drawable);
    window.display();

    // Check updated pixels (blue triangle at position 20-30)
    Image updated_image = window.readPixels();
    const Vector2f new_triangle_center = (new_triangle_base_left + new_triangle_base_right + new_triangle_top) / 3.0f;
    const Vector2i new_triangle_center_check = static_cast<Vector2i>(new_triangle_center);
    const Vector2i new_triangle_left_check = static_cast<Vector2i>(new_triangle_base_left + Vector2f(2, 2));

    // Original position should now be black
    T_COMPARE(updated_image.getPixel(triangle_center_check), Color::Black, &Color::toString);
    T_COMPARE(updated_image.getPixel(triangle_left_check), Color::Black, &Color::toString);

    // New position should be blue
    T_COMPARE(updated_image.getPixel(new_triangle_center_check), Color::Blue, &Color::toString);
    T_COMPARE(updated_image.getPixel(new_triangle_left_check), Color::Blue, &Color::toString);

    // Position outside both triangles should be black
    T_COMPARE(updated_image.getPixel(outside_initial), Color::Black, &Color::toString);
}

void VertexBufferTestsModule::vertexBufferPartialUpdateTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("vertex buffer partial update");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);
    window.clear(Color::Black);

    // Create a VertexBuffer with initial vertices (2 triangles: first 3 vertices red, last 3 vertices blue)
    VertexBuffer vertexBuffer(PrimitiveType::Triangles);
    const std::size_t initialVertexCount = 6;
    T_ASSERT(vertexBuffer.create(initialVertexCount));

    const Vector2f first_triangle_base_left = Vector2f(0, 0);
    const Vector2f first_triangle_base_right = Vector2f(10, 0);
    const Vector2f first_triangle_top = Vector2f(5, 10);
    const Vector2f second_triangle_base_left = Vector2f(20, 0);
    const Vector2f second_triangle_base_right = Vector2f(30, 0);
    const Vector2f second_triangle_top = Vector2f(25, 10);

    std::vector<Vertex> initialVertices = {
        // First triangle (red)
        Vertex(first_triangle_base_left, Color::Red, Vector2f(0, 0)),
        Vertex(first_triangle_base_right, Color::Red, Vector2f(0, 0)),
        Vertex(first_triangle_top, Color::Red, Vector2f(0, 0)),
        // Second triangle (blue)
        Vertex(second_triangle_base_left, Color::Blue, Vector2f(0, 0)),
        Vertex(second_triangle_base_right, Color::Blue, Vector2f(0, 0)),
        Vertex(second_triangle_top, Color::Blue, Vector2f(0, 0))
    };

    T_ASSERT(vertexBuffer.update(initialVertices));

    // Create drawable wrapper and render
    VertexBufferDrawable drawable(vertexBuffer);
    window.draw(drawable);
    window.display();

    // Check initial pixels
    Image initial_image = window.readPixels();
    const Vector2f first_triangle_center = (
        first_triangle_base_left + first_triangle_base_right + first_triangle_top
    ) / 3.0f;
    const Vector2i first_triangle_center_check = static_cast<Vector2i>(first_triangle_center);
    const Vector2f second_triangle_center = (
        second_triangle_base_left + second_triangle_base_right + second_triangle_top
    ) / 3.0f;
    const Vector2i second_triangle_center_check = static_cast<Vector2i>(second_triangle_center);
    const Vector2i outside = static_cast<Vector2i>(first_triangle_base_right + Vector2f(5, 0));

    T_COMPARE(initial_image.getPixel(first_triangle_center_check), Color::Red, &Color::toString);
    T_COMPARE(initial_image.getPixel(second_triangle_center_check), Color::Blue, &Color::toString);
    T_COMPARE(initial_image.getPixel(outside), Color::Black, &Color::toString);

    // Partial update: replace the second triangle (last 3 vertices) with green
    std::vector<Vertex> updateVertices = {
        Vertex(second_triangle_base_left, Color::Green, Vector2f(0, 0)),
        Vertex(second_triangle_base_right, Color::Green, Vector2f(0, 0)),
        Vertex(second_triangle_top, Color::Green, Vector2f(0, 0))
    };

    // Update only the last 3 vertices (offset 3), vertexCount 3
    T_ASSERT(vertexBuffer.update(updateVertices, 3, 3));

    // Render updated buffer
    window.clear(Color::Black);
    window.draw(drawable);
    window.display();

    // Check partial update pixels
    Image updated_image = window.readPixels();

    // First triangle should still be red (not affected by partial update)
    T_COMPARE(updated_image.getPixel(first_triangle_center_check), Color::Red, &Color::toString);

    // Second triangle should now be green (was updated)
    T_COMPARE(updated_image.getPixel(second_triangle_center_check), Color::Green, &Color::toString);

    // Position outside should still be black
    T_COMPARE(updated_image.getPixel(outside), Color::Black, &Color::toString);
}
