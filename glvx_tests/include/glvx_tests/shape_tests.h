#pragma once

#include "glvx_tests/glvx_tests_common.h"

using namespace glvx;

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
    void convexShapeTest(test::Test& test);
    void convexShapeSetPointTest(test::Test& test);
    void convexShapeSetPointCountTest(test::Test& test);
    void convexShapeEmptyTest(test::Test& test);

private:

};
