#include "glvis_tests/render_states_tests.h"

#include "glvis/render_states.h"
#include "glvis/shader.h"
#include "glvis/texture.h"

RenderStatesTestsModule::RenderStatesTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto render_states_transform_test = addTest("render_states_transform", [&](test::Test& test) { renderStatesTransformTest(test); });
    auto render_states_texture_test = addTest("render_states_texture", [&](test::Test& test) { renderStatesTextureTest(test); });
    auto render_states_shader_test = addTest("render_states_shader", [&](test::Test& test) { renderStatesShaderTest(test); });
}

void RenderStatesTestsModule::renderStatesTransformTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("render states transform");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // Render a rectangle with translation using RenderStates
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    const Vector2f transform_offset = Vector2f(10.0f, 10.0f);
    const Vector2i transform_offset_int = static_cast<Vector2i>(transform_offset);
    const Color rect_color = Color::Red;
    Rectangle rect(rect_size);
    rect.setColor(rect_color);
    RenderStates states;
    Matrix4 transform = Matrix4::translate(Matrix4(), Vector3(transform_offset.x, transform_offset.y, 0.0f));
    states.transform = transform;
    window.draw(rect, states);
    window.display();

    // Check that the rectangle is rendered at translated position
    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(
        test,
        image,
        transform_offset_int.x,
        transform_offset_int.y,
        transform_offset_int.x + rect_size_int.x,
        transform_offset_int.y + rect_size_int.y,
        rect_color
    ));

    // Check outside the rectangle
    T_COMPARE(image.getPixel(transform_offset_int - Vector2i(1, 1)), Color::Black, &Color::toString);
    T_COMPARE(image.getPixel(transform_offset_int + rect_size_int), Color::Black, &Color::toString);
}

void RenderStatesTestsModule::renderStatesTextureTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("render states texture");
    View view;
    Vector2f window_center = window.getCenter();
    view.setPosition(window_center);
    window.setView(view);

    // Create a 2x2 texture
    const Vector2i texture_size = Vector2i(2, 2);
    unsigned char texture_data[16] = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };

    // Create a rectangle
    const Vector2f rect_size = static_cast<Vector2f>(texture_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);

    // Render a rectangle without the texture
    window.clear(Color::Black);
    window.draw(rect);
    window.display();

    // Check that the rectangle is rendered correctly
    T_WRAP_CONTAINER(checkPixelColor(test, window.readPixels(), Vector2i(), texture_size, Color::Red));

    // Use RenderStates to set the texture
    Texture tex(texture_data, texture_size.x, texture_size.y);
    RenderStates states;
    states.texture = &tex;
    rect.setColor(Color::White);
    window.clear(Color::Black);
    window.draw(rect, states);
    window.display();

    // Check that the texture is rendered correctly
    Image image = window.readPixels();
    T_COMPARE(image.getPixel(0, 0), Color(1, 2, 3, 4), &Color::toString);
    T_COMPARE(image.getPixel(1, 0), Color(5, 6, 7, 8), &Color::toString);
    T_COMPARE(image.getPixel(0, 1), Color(9, 10, 11, 12), &Color::toString);
    T_COMPARE(image.getPixel(1, 1), Color(13, 14, 15, 16), &Color::toString);

    // Check outside of the texture
    T_COMPARE(image.getPixel(texture_size), Color::Black, &Color::toString);
}

void RenderStatesTestsModule::renderStatesShaderTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("render states shader");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);
    window.clear(Color::Black);

    // Custom shader that outputs green color
    const char* custom_vert = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec4 aColor;
        layout (location = 2) in vec2 aTexCoords;

        out vec2 TexCoords;
        out vec4 VertexColor;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
            TexCoords = aTexCoords;
            VertexColor = aColor;
        }
    )";

    const char* custom_frag = R"(
        #version 330 core

        in vec2 TexCoords;
        in vec4 VertexColor;

        out vec4 FragColor;

        uniform sampler2D tex;
        uniform bool hasTexture;
        uniform vec4 color;

        void main() {
            // Always output green color regardless of input color
            FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    )";

    Shader customShader(custom_vert, custom_frag);

    // Render a rectangle with default shader
    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);
    window.draw(rect);
    window.display();

    // Check that the rectangle is rendered with default shader (red)
    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Red));
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);

    // Render a rectangle with custom shader using RenderStates
    RenderStates states;
    states.shader = &customShader;
    window.clear(Color::Black);
    window.draw(rect, states);
    window.display();

    // Check that the rectangle is rendered with custom shader (green)
    image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Green));
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);
}
