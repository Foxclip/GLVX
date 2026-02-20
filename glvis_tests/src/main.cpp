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
#include "glvis/render_states.h"
#include "glvis/shader.h"
#include "glvis/vector.h"
#include "glvis/color.h"
#include "glvis/vertex.h"
#include "glvis/rectangle.h"
#include "glvis/circle.h"
#include "glvis/window.h"
#include "glvis/texture.h"
#include "glvis/angle.h"
#include "glvis/vertex_array.h"
#include "glvis/vertex_buffer.h"
#include "glvis/utils.h"

using namespace glvis;

class GlvisTestModule : public test::TestModule {
public:
    GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes = { });

private:
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
: test::TestModule(name, parent, required_nodes) {
    window.create(WINDOW_SIZE.x, WINDOW_SIZE.y, "glvis tests");
    auto basic_tests_module = addModule<BasicTestsModule>("Basic");
    auto shape_tests_module = addModule<ShapeTestsModule>("Shape", { basic_tests_module });
    auto transform_tests_module = addModule<TransformTestsModule>("Transform", { shape_tests_module });
    auto texture_tests_module = addModule<TextureTestsModule>("Texture", { shape_tests_module });
    auto window_tests_module = addModule<WindowTestsModule>("Window", { shape_tests_module });
    auto view_tests_module = addModule<ViewTestsModule>("View", { shape_tests_module });
    auto vertex_buffer_tests_module = addModule<VertexBufferTestsModule>("VertexBuffer", { basic_tests_module });
    auto vertex_array_tests_module = addModule<VertexArrayTestsModule>("VertexArray", { vertex_buffer_tests_module });
    auto render_states_tests_module = addModule<RenderStatesTestsModule>("RenderStates", { shape_tests_module, texture_tests_module });
    auto coordinate_transform_tests_module = addModule<CoordinateTransformTestsModule>("CoordinateTransform", { view_tests_module });
}

int main() {
    test::TestModule root("glvis tests", nullptr);
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();
    root.printSummary();

    // TODO: split tests into separate files
    // TODO: generate documentation and README.md
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
