#include <iostream>
#include <cmath>
#include "glvis_tests/glvis_tests_common.h"
#include "glvis_tests/basic_tests.h"
#include "glvis_tests/shape_tests.h"
#include "glvis_tests/transform_tests.h"
#include "glvis_tests/texture_tests.h"
#include "glvis_tests/window_tests.h"
#include "glvis_tests/view_tests.h"
#include "glvis_tests/vertex_buffer_tests.h"
#include "glvis_tests/vertex_array_tests.h"
#include "glvis_tests/render_states_tests.h"
#include "glvis_tests/coordinate_transform_tests.h"
#include "glvis_tests/text_tests.h"

int main() {
    test::TestModule root("glvis tests", nullptr);
    window.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "glvis tests");
    auto basic_tests_module = root.addModule<BasicTestsModule>("Basic");
    auto shape_tests_module = root.addModule<ShapeTestsModule>("Shape", { basic_tests_module });
    auto transform_tests_module = root.addModule<TransformTestsModule>("Transform", { shape_tests_module });
    auto texture_tests_module = root.addModule<TextureTestsModule>("Texture", { shape_tests_module });
    auto window_tests_module = root.addModule<WindowTestsModule>("Window", { shape_tests_module });
    auto view_tests_module = root.addModule<ViewTestsModule>("View", { shape_tests_module });
    auto vertex_buffer_tests_module = root.addModule<VertexBufferTestsModule>("VertexBuffer", { basic_tests_module });
    auto vertex_array_tests_module = root.addModule<VertexArrayTestsModule>("VertexArray", { vertex_buffer_tests_module });
    auto render_states_tests_module = root.addModule<RenderStatesTestsModule>("RenderStates", { shape_tests_module, texture_tests_module });
    auto coordinate_transform_tests_module = root.addModule<CoordinateTransformTestsModule>("CoordinateTransform", { view_tests_module });
    auto text_tests_module = root.addModule<TextTestsModule>("Text", { shape_tests_module });
    root.run();
    root.printSummary();

    // TODO: Font: create font texture in swizzled form right away
    // TODO: text rendering
    // TODO: AbstractTexture: add option to not blit old contents when resizing
    // TODO: add RenderTexture test
    // TODO: Textures: add parameters for interpolation
    // TODO: remove path from Texture
    // TODO: use uniform buffer objects
    // TODO: transparent texture rendering

    return 0;
}
