#pragma once

#include "test_lib/test.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

extern Window aaWindow;
const int AA_WINDOW_SAMPLES = 4;

class AntialiasingTestsModule : public test::TestModule {
public:
    AntialiasingTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void windowAASolidRectTest(test::Test& test);
    void windowAASubpixelRectTest(test::Test& test);
    void renderTextureAASolidRectTest(test::Test& test);
    void renderTextureAASubpixelRectTest(test::Test& test);

private:

    void beforeRunModule() override;

};
