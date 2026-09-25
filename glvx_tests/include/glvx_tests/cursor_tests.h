#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

class CursorTestsModule : public test::TestModule {
public:
    CursorTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void cursorSystemAllTypesTest(test::Test& test);
    void cursorPixelsLoadTest(test::Test& test);
    void cursorPixelsInvalidTest(test::Test& test);
    void cursorSetOnWindowTest(test::Test& test);

private:

    void beforeRunModule() override;
    void afterRunModule() override;

};
