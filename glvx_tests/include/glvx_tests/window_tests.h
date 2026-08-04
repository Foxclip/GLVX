#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

class WindowTestsModule : public test::TestModule {
public:
    WindowTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void windowResizeTest(test::Test& test);

private:

};
