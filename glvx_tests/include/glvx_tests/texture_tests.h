#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

class TextureTestsModule : public test::TestModule {
public:
    TextureTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void textureTest(test::Test& test);
    void textureAlphaTest(test::Test& test);
    void textureColorMultiplyTest(test::Test& test);
    void textureResizeTest(test::Test& test);
    void textureInterpolationTest(test::Test& test);
    void textureRenderingInterpolationTest(test::Test& test);
    void textureWrappingTest(test::Test& test);

};
