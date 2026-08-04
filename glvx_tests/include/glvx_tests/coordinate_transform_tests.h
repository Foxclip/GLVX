#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

class CoordinateTransformTestsModule : public test::TestModule {
public:
    CoordinateTransformTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void worldToScreenIdentityTest(test::Test& test);
    void worldToScreenPanTest(test::Test& test);
    void worldToScreenZoomTest(test::Test& test);
    void worldToScreenRotateTest(test::Test& test);
    void screenToWorldIdentityTest(test::Test& test);
    void screenToWorldPanTest(test::Test& test);
    void screenToWorldZoomTest(test::Test& test);
    void screenToWorldRotateTest(test::Test& test);
    void coordinateRoundTripTest(test::Test& test);

private:

};
