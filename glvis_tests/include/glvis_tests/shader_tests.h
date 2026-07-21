#pragma once

#include "test_lib/test.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class ShaderTestsModule : public test::TestModule {
public:
    ShaderTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void shaderCombinedTest(test::Test& test);

private:

};
