#pragma once

#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class TransformTestsModule : public test::TestModule {
public:
    TransformTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );
    void moveTest(test::Test& test);
    void setOriginTest(test::Test& test);
    void rotateTopLeftTest(test::Test& test);
    void rotateCenterTest(test::Test& test);
    void scaleTopLeftTest(test::Test& test);
    void scaleCenterTest(test::Test& test);

private:

};
