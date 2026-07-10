#pragma once

#include "test_lib/test.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class RenderTextureTestsModule : public test::TestModule {
public:
    RenderTextureTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void clearTest(test::Test& test);
    void renderTexturePanTest(test::Test& test);
    void transparentRectangleTest(test::Test& test);

private:

};
