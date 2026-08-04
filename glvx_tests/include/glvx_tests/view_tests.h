#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

class ViewTestsModule : public test::TestModule {
public:
    ViewTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void viewPanTest(test::Test& test);
    void viewZoomTest(test::Test& test);
    void viewRotateTest(test::Test& test);

private:

};
