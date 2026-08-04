#pragma once

#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class InputTestsModule : public test::TestModule {
public:
    InputTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void mouseMoveTest(test::Test& test);
    void mouseButtonPressedTest(test::Test& test);
    void mouseButtonReleasedTest(test::Test& test);
    void mouseWheelScrolledTest(test::Test& test);

    void keyPressedTest(test::Test& test);
    void keyReleasedTest(test::Test& test);
    void keyPressedWithModifierTest(test::Test& test);
    void textEnteredTest(test::Test& test);
    void keyboardStateTest(test::Test& test);
};
