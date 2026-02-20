#pragma once

#include "test_lib/test.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class RenderStatesTestsModule : public test::TestModule {
public:
    RenderStatesTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void renderStatesTransformTest(test::Test& test);
    void renderStatesTextureTest(test::Test& test);
    void renderStatesShaderTest(test::Test& test);

private:

};
