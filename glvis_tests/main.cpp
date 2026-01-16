#include <iostream>
#include <test_lib/test.h>
#include "glvis/vector.h"
#include "glvis/color.h"
#include "glvis/vertex.h"
#include "glvis/window.h"

class GlvisTestModule : public test::TestModule {
public:
    GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes = { });

private:
    void basicTest(test::Test& test);
};

GlvisTestModule::GlvisTestModule(const std::string& name, test::TestModule* parent, const std::vector<test::TestNode*>& required_nodes)
    : test::TestModule(name, parent, required_nodes) {
    addTest("basic", [&](test::Test& test) { basicTest(test); });
}

void GlvisTestModule::basicTest(test::Test& test) {
    glvis::Window window;
    window.create(800, 600, "GLVis Test");
}

int main() {
    test::TestModule root("GLVis Tests", nullptr);
    root.print_summary_enabled = true;
    GlvisTestModule* glvisModule = root.addModule<GlvisTestModule>("Basic");
    root.run();

    // TODO: make rendering tests
    // TODO: text rendering
    // TODO: transparent texture rendering

    return 0;
}
