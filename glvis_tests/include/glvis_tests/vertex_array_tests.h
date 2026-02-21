#pragma once

#include "test_lib/test.h"
#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class VertexArrayTestsModule : public test::TestModule {
public:
    VertexArrayTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void vertexArrayTriangleTest(test::Test& test);
    void vertexArrayLineTest(test::Test& test);
    void vertexArrayModifyTest(test::Test& test);

private:

};
