#pragma once

#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class ShapeTestsModule : public test::TestModule {
public:
    ShapeTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void rectangleTest(test::Test& test);
    void rectangleSetSizeTest(test::Test& test);
    void circleTest(test::Test& test);
    void circleSetRadiusTest(test::Test& test);
    void transparentRectangleTest(test::Test& test);
    void multipleTransparentRectanglesTest(test::Test& test);

private:

};
