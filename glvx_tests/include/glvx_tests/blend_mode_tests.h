#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

class BlendModeTestsModule : public test::TestModule {
public:
    BlendModeTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void blendModeDefaultTest(test::Test& test);
    void blendModeAlphaTest(test::Test& test);
    void blendModeAddTest(test::Test& test);
    void blendModeMultiplyTest(test::Test& test);
    void blendModeNoneTest(test::Test& test);
    void blendModeOperatorEqualTest(test::Test& test);
    void blendModeCustomTest(test::Test& test);

private:

};
