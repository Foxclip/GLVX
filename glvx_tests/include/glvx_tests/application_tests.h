#pragma once

#include "glvx_tests/glvx_tests_common.h"

class ApplicationTestsModule : public test::TestModule {
public:
    ApplicationTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void initTest(test::Test& test);
    void clearTest(test::Test& test);
    void rectangleTest(test::Test& test);
    void moveRectangleTest(test::Test& test);

private:

};
