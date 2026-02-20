#pragma once

#include "test_lib/test.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class TextureTestsModule : public test::TestModule {
public:
    TextureTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void textureTest(test::Test& test);
    void textureColorMultiplyTest(test::Test& test);
    void textureResizeTest(test::Test& test);

private:

};
