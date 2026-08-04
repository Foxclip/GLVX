#pragma once

#include "glvis_tests/glvis_tests_common.h"

using namespace glvis;

class VertexBufferTestsModule : public test::TestModule {
public:
    VertexBufferTestsModule(
        const std::string& name,
        test::TestModule* parent,
        const std::vector<test::TestNode*>& required_nodes = { }
    );

    void vertexBufferRenderTest(test::Test& test);
    void vertexBufferUpdateTest(test::Test& test);
    void vertexBufferPartialUpdateTest(test::Test& test);

private:

};
