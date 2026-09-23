#pragma once

#include "glvx_tests/glvx_tests_common.h"
#include "glvx/float_rect.h"

using namespace glvx;

class FloatRectTestsModule : public test::TestModule {
public:
    FloatRectTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void intersectsOverlapTest(test::Test& test);
    void intersectsNoOverlapTest(test::Test& test);
    void intersectsTouchingEdgesTest(test::Test& test);
    void intersectsContainedTest(test::Test& test);
    void intersectsResultTest(test::Test& test);

private:

};
