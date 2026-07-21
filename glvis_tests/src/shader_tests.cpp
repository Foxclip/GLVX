#include "glvis_tests/shader_tests.h"
#include "glvis/rectangle.h"
#include "glvis/render_states.h"
#include "glvis/shader.h"

ShaderTestsModule::ShaderTestsModule(
    const std::string& name,
    test::TestModule* parent,
    const std::vector<test::TestNode*>& required_nodes
) : test::TestModule(name, parent, required_nodes) {
    auto shader_combined_test = addTest("shader_combined", [&](test::Test& test) { shaderCombinedTest(test); });
}

void ShaderTestsModule::shaderCombinedTest(test::Test& test) {
    window.setSize(WINDOW_SIZE);
    window.setTitle("shader combined");
    View view;
    view.setPosition(window.getCenter());
    window.setView(view);

    const char* vert = R"(
        #version 420 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec4 aColor;
        layout (location = 2) in vec2 aTexCoords;

        out vec4 VertexColor;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
            VertexColor = aColor;
        }
    )";

    const char* frag_template = R"(
        #version 420 core

        in vec4 VertexColor;

        out vec4 FragColor;

        %INCLUDE%

        void main() {
            vec4 color = VertexColor;

            %APPLY%

            FragColor = color;
        }
    )";

    const char* blue_part_source = R"(
        vec4 blue_apply(vec4 c) {
            return vec4(0.0, 0.0, 1.0, 1.0);
        }
    )";

    std::vector<ShaderPart> parts = { {"blue", blue_part_source} };

    Shader combinedShader(vert, frag_template, parts);

    const Vector2f rect_size = Vector2f(10.0f, 10.0f);
    const Vector2i rect_size_int = static_cast<Vector2i>(rect_size);
    Rectangle rect(rect_size);
    rect.setColor(Color::Red);

    window.clear(Color::Black);
    RenderStates states;
    states.shader = &combinedShader;
    window.draw(rect, states);
    window.display();

    Image image = window.readPixels();
    T_WRAP_CONTAINER(checkPixelColor(test, image, Vector2i(), rect_size_int, Color::Blue));
    T_COMPARE(image.getPixel(rect_size_int), Color::Black, &Color::toString);
}
