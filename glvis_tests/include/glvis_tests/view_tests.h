#pragma once

#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

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
