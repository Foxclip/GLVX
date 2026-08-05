#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

class ApplicationTestsModule : public test::TestModule {
public:
    ApplicationTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void clearTest(test::Test& test);

private:

};
