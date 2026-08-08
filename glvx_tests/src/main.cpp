#include <iostream>
#include "glvx_tests/glvx_tests_common.h"
#include "glvx_tests/basic_tests.h"
#include "glvx_tests/shape_tests.h"
#include "glvx_tests/transform_tests.h"
#include "glvx_tests/texture_tests.h"
#include "glvx_tests/window_tests.h"
#include "glvx_tests/view_tests.h"
#include "glvx_tests/vertex_buffer_tests.h"
#include "glvx_tests/vertex_array_tests.h"
#include "glvx_tests/render_states_tests.h"
#include "glvx_tests/blend_mode_tests.h"
#include "glvx_tests/coordinate_transform_tests.h"
#include "glvx_tests/text_tests.h"
#include "glvx_tests/render_texture_tests.h"
#include "glvx_tests/shader_tests.h"
#include "glvx_tests/antialiasing_tests.h"
#include "glvx_tests/input_tests.h"
#include "glvx_tests/application_tests.h"

int main() {

    try {

        test::TestModule root("glvx tests", nullptr);
        window.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "glvx tests");
        auto basic_tests_module = root.addModule<BasicTestsModule>("Basic");
        auto shape_tests_module = root.addModule<ShapeTestsModule>("Shape", { basic_tests_module });
        auto transform_tests_module = root.addModule<TransformTestsModule>("Transform", { shape_tests_module });
        auto texture_tests_module = root.addModule<TextureTestsModule>("Texture", { shape_tests_module });
        auto window_tests_module = root.addModule<WindowTestsModule>("Window", { shape_tests_module });
        auto view_tests_module = root.addModule<ViewTestsModule>("View", { shape_tests_module });
        auto vertex_buffer_tests_module = root.addModule<VertexBufferTestsModule>("VertexBuffer", { basic_tests_module });
        auto vertex_array_tests_module = root.addModule<VertexArrayTestsModule>("VertexArray", { vertex_buffer_tests_module });
        auto render_states_tests_module = root.addModule<RenderStatesTestsModule>("RenderStates", { shape_tests_module, texture_tests_module });
        auto blend_mode_tests_module = root.addModule<BlendModeTestsModule>("BlendMode", { shape_tests_module });
        auto coordinate_transform_tests_module = root.addModule<CoordinateTransformTestsModule>("CoordinateTransform", { view_tests_module });
        auto text_tests_module = root.addModule<TextTestsModule>("Text", { shape_tests_module });
        auto render_texture_tests_module = root.addModule<RenderTextureTestsModule>("RenderTexture", { basic_tests_module });
        auto shader_tests_module = root.addModule<ShaderTestsModule>("Shader", { shape_tests_module });
        auto antialiasing_tests_module = root.addModule<AntialiasingTestsModule>("Antialiasing", { shape_tests_module });
        auto input_tests_module = root.addModule<InputTestsModule>("Input", { basic_tests_module });
        auto application_tests_module = root.addModule<ApplicationTestsModule>("Application", { input_tests_module });
        root.run();
        root.printSummary();

    } catch (const std::string& str) {
        std::cerr << "ERROR: " << str << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    // TODO: FloatRect: add intersects()
    // TODO: Drawable: add texture rect

    return 0;
}
